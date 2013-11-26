/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/BeSurround.cxx
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
#include "BeSurround.h"

namespace delftSystem
{

BeSurround::BeSurround(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  active(1),insertIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insertIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeSurround::BeSurround(const BeSurround& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  active(A.active),insertIndex(A.insertIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  length(A.length),mat(A.mat)
  /*!
    Copy constructor
    \param A :: BeSurround to copy
  */
{}

BeSurround&
BeSurround::operator=(const BeSurround& A)
  /*!
    Assignment operator
    \param A :: BeSurround to copy
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
      outerRadius=A.outerRadius;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}

BeSurround::~BeSurround() 
 /*!
   Destructor
 */
{}

void
BeSurround::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BeSurround","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");


  length=Control.EvalVar<double>(keyName+"Length");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  
  active=Control.EvalDefVar<int>(keyName+"Active",1);
  return;
}
  
void
BeSurround::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
  */
{
  ELog::RegMethod RegA("BeSurround","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin+=X*xStep+Y*yStep+Z*zStep;

  return;
}

void
BeSurround::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeSurround","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,insertIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,insertIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,insertIndex+7,
			      Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,insertIndex+17,
			      Origin,Y,outerRadius);

  return;
}

void
BeSurround::createObjects(Simulation& System,
			  const std::string& ExcludeString)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeSurround","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,insertIndex," 1 -2 -17 7 ");
  addOuterSurf(Out);
  Out+=ExcludeString;
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  return;
}

void
BeSurround::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin+X*outerRadius+Y*length/2.0,X);

  FixedComp::setLinkSurf(0,SMap.realSurf(insertIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(insertIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(insertIndex+17));

  return;
}

void
BeSurround::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		      const std::string& ExcludeStr)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
  */
{
  ELog::RegMethod RegA("BeSurround","createAll");
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
