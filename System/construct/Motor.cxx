/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Motor.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "boltRing.h"
#include "Motor.h"

namespace constructSystem
{

Motor::Motor(const std::string& Key) : 
  attachSystem::FixedOffset(Key,4),
  attachSystem::ContainedGroup("Axle","Plate","Outer"),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  motorIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(motorIndex+1),frontInner(0),backInner(0),revFlag(0),
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
  attachSystem::FixedOffset(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  motorIndex(A.motorIndex),cellIndex(A.cellIndex),
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
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

  FixedOffset::populate(Control);
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
Motor::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Motor","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
Motor::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Motor","createSurfaces");

  ModelSupport::buildCylinder(SMap,motorIndex+7,Origin,Y,axleRadius);
  addSurf("axle",SMap.realSurf(motorIndex+7));
  ModelSupport::buildCylinder(SMap,motorIndex+107,Origin,Y,bodyRadius);
  
  ModelSupport::buildPlane(SMap,motorIndex+1,Origin-Y*(yFront+plateThick),Y);
  ModelSupport::buildPlane(SMap,motorIndex+2,Origin+Y*(yBack+plateThick),Y);

  if (!revFlag)
    ModelSupport::buildPlane(SMap,motorIndex+11,
			     Origin-Y*(yFront+plateThick+bodyLength),Y);
  else
    ModelSupport::buildPlane(SMap,motorIndex+12,
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

  const std::string axle=
    ModelSupport::getComposite(SMap,motorIndex," 7 ");

  std::string Out;
  
  frontPlate->setFront(SMap.realSurf(motorIndex+1));
  frontPlate->setBack(-frontInner);
  frontPlate->createAll(System,*this,0);

  backPlate->setFront(-backInner);
  backPlate->setBack(-SMap.realSurf(motorIndex+2));
  backPlate->createAll(System,*this,0);

  const std::string FPlate=
    frontPlate->frontRule()+frontPlate->backRule();
  const std::string BPlate=
    backPlate->frontRule()+backPlate->backRule();
  
  Out=frontPlate->getSurfComplement("innerRing");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plateMat,0.0,
				   Out+FPlate+axle));
  addCell("MotorFrontPlate",cellIndex-1);

  Out=backPlate->getSurfComplement("innerRing");
  System.addCell(MonteCarlo::Qhull(cellIndex++,plateMat,0.0,
				   Out+BPlate+axle));
  addCell("MotorBackPlate",cellIndex-1);

  // Note plates can be different sizes:
  Out=frontPlate->getSurfComplement("outerRing");
  addOuterSurf("Outer",Out+FPlate);
  addOuterSurf("Plate",Out+FPlate);
  
  Out=backPlate->getSurfComplement("outerRing");
  addOuterUnionSurf("Plate",Out+BPlate);
  addOuterUnionSurf("Outer",Out+BPlate);

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
  
  std::string Out;

  // Axle
  Out=ModelSupport::getComposite(SMap,motorIndex," 1 -2 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,axleMat,0.0,Out));
  addCell("Axle",cellIndex-1);
    
  // Main motor
  if (!revFlag)
    Out=ModelSupport::getComposite(SMap,motorIndex," 11 -1 -107 ");
  else
    Out=ModelSupport::getComposite(SMap,motorIndex," -12 2 -107 ");
      
  System.addCell(MonteCarlo::Qhull(cellIndex++,bodyMat,0.0,Out));
  addCell("MotorBody",cellIndex-1);

  addOuterUnionSurf("Outer",Out);
  Out=ModelSupport::getComposite(SMap,motorIndex," -7 ");
  addOuterSurf("Axle",Out);
  
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
