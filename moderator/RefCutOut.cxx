/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderator/RefCutOut.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "RefCutOut.h"

namespace moderatorSystem
{

RefCutOut::RefCutOut(const std::string& Key)  :
  attachSystem::FixedComp(Key,1),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RefCutOut::RefCutOut(const RefCutOut& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  xyAngle(A.xyAngle),zAngle(A.zAngle),
  tarLen(A.tarLen),tarOut(A.tarOut),radius(A.radius),matN(A.matN)
  /*!
    Copy constructor
    \param A :: RefCutOut to copy
  */
{}

RefCutOut&
RefCutOut::operator=(const RefCutOut& A)
  /*!
    Assignment operator
    \param A :: RefCutOut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      tarLen=A.tarLen;
      tarOut=A.tarOut;
      radius=A.radius;
      matN=A.matN;
    }
  return *this;
}

RefCutOut::~RefCutOut() 
  /*!
    Destructor
  */
{}

void
RefCutOut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("RefCutOut","populate");
  

  active=Control.EvalDefVar<int>(keyName+"Active",1);
  if (active)
    {
      xyAngle=Control.EvalVar<double>(keyName+"XYAngle"); 
      zAngle=Control.EvalVar<double>(keyName+"ZAngle"); 
      tarLen=Control.EvalVar<double>(keyName+"TargetDepth");
      tarOut=Control.EvalVar<double>(keyName+"TargetOut");
      radius=Control.EvalVar<double>(keyName+"Radius"); 
      
      matN=ModelSupport::EvalMat<int>(Control,keyName+"Mat"); 
    }
  
  return;
}
  

void
RefCutOut::createUnitVector(const attachSystem::FixedComp& CUnit)
  /*!
    Create the unit vectors
    - Y Points down Target
    - X Across the target
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("RefCutOut","createUnitVector");
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  FixedComp::createUnitVector(CUnit);

  const masterRotate& MR=masterRotate::Instance();
  Origin+=Y*tarLen;
  


  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);
  
  CS.setCNum(chipIRDatum::chipStartPt,MR.calcRotate(Origin));
  CS.setCNum(chipIRDatum::tubeAxis,MR.calcAxisRotate(Y));

  return;
}

void
RefCutOut::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RefCutOut","createSurface");

  ModelSupport::buildPlane(SMap,pipeIndex+1,Origin+Y*tarOut,Y);
  ModelSupport::buildCylinder(SMap,pipeIndex+7,Origin+Y*tarOut,Y,radius);
  return;
}

void
RefCutOut::createObjects(Simulation& System)
  /*!
    Adds the Cylinder
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RefCutOut","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,pipeIndex," 1 -7 ");
  addOuterSurf(Out);

  // Inner Void
  Out+=" "+ContainedComp::getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
}

  
void
RefCutOut::createAll(Simulation& System,
		     const attachSystem::FixedComp& FUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("RefCutOut","createAll");
  populate(System.getDataBase());

  if (active)
    {
      createUnitVector(FUnit);
      createSurfaces();
      createObjects(System);
      insertObjects(System);       
    }
  return;
}
  
}  // NAMESPACE moderatorSystem
