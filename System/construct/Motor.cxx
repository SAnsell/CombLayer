/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Motor.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "boltRing.h"
#include "Motor.h"

namespace constructSystem
{

Motor::Motor(const std::string& Key) : 
  attachSystem::FixedRotate(Key,4),
  attachSystem::ContainedGroup("Axle","Plate","Outer"),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  frontInner(0),backInner(0),revFlag(0),
  yFront(0.0),yBack(0.0),
  frontPlate(new constructSystem::boltRing(Key,"FrontPlate")),
  backPlate(new constructSystem::boltRing(Key,"BackPlate"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  OR.addObject(frontPlate);
  OR.addObject(backPlate);

}

Motor::Motor(const Motor& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  frontInner(A.frontInner),backInner(A.backInner),
  revFlag(A.revFlag),bodyLength(A.bodyLength),
  plateThick(A.plateThick),axleRadius(A.axleRadius),
  portInnerRadius(A.portInnerRadius),
  portOuterRadius(A.portOuterRadius),boltRadius(A.boltRadius),
  bodyRadius(A.bodyRadius),nBolt(A.nBolt),
  angOffset(A.angOffset),boltMat(A.boltMat),
  bodyMat(A.bodyMat),axleMat(A.axleMat),plateMat(A.plateMat),
  yFront(A.yFront),yBack(A.yBack),frontPlate(A.frontPlate),
  backPlate(A.backPlate)
  /*!
    Copy constructor
    \param A :: Motor to copy
  */
{}

Motor&
Motor::operator=(const Motor& A)
  /*!
    Assignment operator
    \param A :: Motor to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      frontInner=A.frontInner;
      backInner=A.backInner;
      revFlag=A.revFlag;
      bodyLength=A.bodyLength;
      plateThick=A.plateThick;
      axleRadius=A.axleRadius;
      portInnerRadius=A.portInnerRadius;
      portOuterRadius=A.portOuterRadius;
      boltRadius=A.boltRadius;
      bodyRadius=A.bodyRadius;
      nBolt=A.nBolt;
      angOffset=A.angOffset;
      boltMat=A.boltMat;
      bodyMat=A.bodyMat;
      axleMat=A.axleMat;
      plateMat=A.plateMat;
      yFront=A.yFront;
      yBack=A.yBack;
      frontPlate=A.frontPlate;
      backPlate=A.backPlate;
    }
  return *this;
}

Motor::~Motor() 
  /*!
    Destructor
  */
{}

void
Motor::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Motor","populate");

  FixedRotate::populate(Control);
  //  + Fe special:
  bodyLength=Control.EvalVar<double>(keyName+"BodyLength");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  
  revFlag=Control.EvalDefVar<int>(keyName+"Reverse",0);
  bodyRadius=Control.EvalVar<double>(keyName+"BodyRadius");
  axleRadius=Control.EvalVar<double>(keyName+"AxleRadius");

  //  boltMat=ModelSupport::EvalMat<int>(Control,keyName+"BoltMat");
  bodyMat=ModelSupport::EvalMat<int>(Control,keyName+"BodyMat");
  axleMat=ModelSupport::EvalMat<int>(Control,keyName+"AxleMat");
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");

  yFront=Control.EvalDefVar<double>(keyName+"YFront",yFront);
  yBack=Control.EvalDefVar<double>(keyName+"YBack",yBack);
  
  return;
}

void
Motor::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Motor","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,axleRadius);
  addSurf("axle",SMap.realSurf(buildIndex+7));
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,bodyRadius);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(yFront+plateThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(yBack+plateThick),Y);

  if (!revFlag)
    ModelSupport::buildPlane(SMap,buildIndex+11,
			     Origin-Y*(yFront+plateThick+bodyLength),Y);
  else
    ModelSupport::buildPlane(SMap,buildIndex+12,
			     Origin+Y*(yBack+plateThick+bodyLength),Y);

  return;
}

void
Motor::createPlates(Simulation& System)
  /*!
    Create the main plates
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Motor","createPlates");

  const HeadRule axle=ModelSupport::getHeadRule(SMap,buildIndex,"7");

  HeadRule HR;
  
  frontPlate->setFront(SMap.realSurf(buildIndex+1));
  frontPlate->setBack(-frontInner);
  frontPlate->createAll(System,*this,0);

  backPlate->setFront(-backInner);
  backPlate->setBack(-SMap.realSurf(buildIndex+2));
  backPlate->createAll(System,*this,0);

  const HeadRule FPlate=
    frontPlate->getFrontRule()*frontPlate->getBackRule();
  const HeadRule BPlate=
    backPlate->getFrontRule()*backPlate->getBackRule();
  
  HR=frontPlate->getSurfRules("#innerRing");
  makeCell("MotorFrontPlate",System,cellIndex++,plateMat,0.0,HR*FPlate*axle);

  HR=backPlate->getSurfRules("#innerRing");
  makeCell("MotorBackPlate",System,cellIndex++,plateMat,0.0,HR*BPlate*axle);

  // Note plates can be different sizes:
  HR=frontPlate->getSurfRules("#outerRing");
  addOuterSurf("Outer",HR*FPlate);
  addOuterSurf("Plate",HR*FPlate);
  
  HR=backPlate->getSurfRules("#outerRing");
  addOuterUnionSurf("Plate",HR*BPlate);
  addOuterUnionSurf("Outer",HR*BPlate);

  return;
}
  
void
Motor::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Motor","createObjects");
  
  HeadRule HR;

  // Axle
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  makeCell("Axle",System,cellIndex++,axleMat,0.0,HR);

    
  // Main motor
  if (!revFlag)
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 -107");
  else
    HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 2 -107");
      
  makeCell("MotorBody",System,cellIndex++,bodyMat,0.0,HR);

  addOuterUnionSurf("Outer",HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  addOuterSurf("Axle",HR);
  
  return;
}

void
Motor::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Motor","createLinks");

  return;
}

void
Motor::createAll(Simulation& System,
		 const attachSystem::FixedComp& motorFC,
		 const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param motorFC :: FixedComp at the motor centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("Motor","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(motorFC,FIndex);
  
  createSurfaces();
  createPlates(System);  
  createObjects(System);  
  //  createLinks();
  insertObjects(System);   


  return;
}
  
}  // NAMESPACE constructSystem
