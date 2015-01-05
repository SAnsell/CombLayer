/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/BeamInsert.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <memory>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "beamSlot.h"
#include "BeamInsert.h"

namespace delftSystem
{

BeamInsert::BeamInsert(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  insertIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insertIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamInsert::BeamInsert(const BeamInsert& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  insertIndex(A.insertIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),length(A.length),
  radius(A.radius),nSlots(A.nSlots),Holes(A.Holes),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: BeamInsert to copy
  */
{}

BeamInsert&
BeamInsert::operator=(const BeamInsert& A)
  /*!
    Assignment operator
    \param A :: BeamInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      length=A.length;
      radius=A.radius;
      nSlots=A.nSlots;
      Holes=A.Holes;
      mat=A.mat;
    }
  return *this;
}


BeamInsert::~BeamInsert() 
 /*!
   Destructor
 */
{}

void
BeamInsert::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("BeamInsert","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  nSlots=Control.EvalVar<size_t>(keyName+"NSlots");

  // Create Holes
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::string::size_type pos=keyName.find("Insert");
  const std::string BIStr(keyName.substr(0,pos));
  for(size_t i=0;i<nSlots;i++)
    {
      Holes.push_back(std::shared_ptr<beamSlot>
		      (new beamSlot(BIStr+"Slot",static_cast<int>(i+1))));
      OR.addObject(StrFunc::makeString(BIStr+"Slot",i+1),Holes.back());


    }
  
  return;
}
  
void
BeamInsert::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: Index on fixed unit
  */
{
  ELog::RegMethod RegA("BeamInsert","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin+=X*xStep+Y*yStep+Z*zStep;
  ELog::EM<<"Origin == "<<Origin<<ELog::endDebug;

  if (fabs(xyAngle)>Geometry::zeroTol || 
      fabs(zAngle)>Geometry::zeroTol)
    {
      const Geometry::Quaternion Qz=
	Geometry::Quaternion::calcQRotDeg(zAngle,X);
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  
      Qz.rotate(Y);
      Qz.rotate(Z);
      Qxy.rotate(Y);
      Qxy.rotate(X);
      Qxy.rotate(Z); 
    }
  return;
}

void
BeamInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamInsert","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,insertIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,insertIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,insertIndex+7,
			      Origin,Y,radius);

  return;
}

void
BeamInsert::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeamInsert","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,insertIndex," 1 -2 -7 ");
  addOuterSurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  return;
}

void
BeamInsert::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  FixedComp::setConnect(0,Origin,-Y);      
  FixedComp::setConnect(1,Origin+Y*length,Y);
  FixedComp::setConnect(2,Origin+X*radius,X);

  FixedComp::setLinkSurf(0,SMap.realSurf(insertIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(insertIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(insertIndex+7));

  return;
}

void
BeamInsert::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
  */
{
  ELog::RegMethod RegA("BeamInsert","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  for(size_t i=0;i<nSlots;i++)
    {
      Holes[i]->addInsertCell(cellIndex-1);
      Holes[i]->createAll(System,*this);
    }
  return;
}

  
}  // NAMESPACE delftSystem
