/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/BeCube.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BeCube.h"

namespace delftSystem
{

BeCube::BeCube(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  active(1),insertIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insertIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeCube::BeCube(const BeCube& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  active(A.active),insertIndex(A.insertIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  innerRadius(A.innerRadius),
  outerWidth(A.outerWidth),outerHeight(A.outerHeight),
  length(A.length),mat(A.mat)
  /*!
    Copy constructor
    \param A :: BeCube to copy
  */
{}

BeCube&
BeCube::operator=(const BeCube& A)
  /*!
    Assignment operator
    \param A :: BeCube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      active=A.active;
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      innerRadius=A.innerRadius;
      outerWidth=A.outerWidth;
      outerHeight=A.outerHeight;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}


BeCube::~BeCube() 
 /*!
   Destructor
 */
{}

void
BeCube::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BeCube","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");


  length=Control.EvalVar<double>(keyName+"Length");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");
  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  
  active=Control.EvalDefVar<int>(keyName+"Active",1);
  return;
}
  
void
BeCube::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
  */
{
  ELog::RegMethod RegA("BeCube","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin+=X*xStep+Y*yStep+Z*zStep;

  return;
}

void
BeCube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeCube","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,insertIndex+1,Origin,Y);

  ModelSupport::buildPlane(SMap,insertIndex+2,Origin+Y*length,Y);
  if (innerRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder(SMap,insertIndex+7,Origin,Y,innerRadius);

  ModelSupport::buildPlane(SMap,insertIndex+13,Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,insertIndex+14,Origin+X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,insertIndex+15,Origin-Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,insertIndex+16,Origin+Z*(outerHeight/2.0),Z);

  return;
}

void
BeCube::createObjects(Simulation& System,
			  const std::string& ExcludeString)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeCube","createObjects");
  
  std::string Out;
  if (innerRadius>Geometry::zeroTol)
    Out=ModelSupport::getComposite(SMap,insertIndex," 1 -2 13 -14 15 -16 7 ");
  else
    Out=ModelSupport::getComposite(SMap,insertIndex," 1 -2 13 -14 15 -16 ");
  addOuterSurf(Out);
  Out+=ExcludeString;
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  return;
}

void
BeCube::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin-X*(outerWidth/2.0)+Y*(length/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(outerWidth/2.0)+Y*(length/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(outerHeight/2.0)+Y*(length/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(outerHeight/2.0)+Y*(length/2.0),Z);

  FixedComp::setLinkSurf(0,SMap.realSurf(insertIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(insertIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(insertIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(insertIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(insertIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(insertIndex+16));

  return;
}

void
BeCube::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		      const std::string& ExcludeStr)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param ExcludeStr :: Extra string to exlude
  */
{
  ELog::RegMethod RegA("BeCube","createAll");
  populate(System);
  if (!active) return;

  createUnitVector(FC);
  createSurfaces();
  createObjects(System,ExcludeStr);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE delftSystem
