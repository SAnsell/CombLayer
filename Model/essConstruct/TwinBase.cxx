/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TwinBase.cxx
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "RingSeal.h"
#include "InnerPort.h"
#include "boltRing.h"
#include "Motor.h"
#include "TwinBase.h"

namespace constructSystem
{

TwinBase::TwinBase(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2,
				 "MotorTop",3,"MotorBase",3),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),
  houseIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(houseIndex+1),
  motorA(new constructSystem::Motor(Key+"MotorA")),
  motorB(new constructSystem::Motor(Key+"MotorB"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(motorA);
  OR.addObject(motorB);
}

TwinBase::~TwinBase() 
  /*!
    Destructor
  */
{}

void
TwinBase::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TwinBase","populate");

  FixedOffsetGroup::populate(Control);
  //  + Fe special:
  stepHeight=Control.EvalVar<double>(keyName+"StepHeight");
  length=Control.EvalVar<double>(keyName+"Length");
  mainRadius=Control.EvalVar<double>(keyName+"MainRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerVoid=Control.EvalVar<double>(keyName+"InnerVoid");
  innerLowStep=Control.EvalVar<double>(keyName+"InnerLowStep");
  innerTopStep=Control.EvalVar<double>(keyName+"InnerTopStep");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  outerRingNBolt=Control.EvalVar<size_t>(keyName+"OuterRingNBolt");
  outerLineNBolt=Control.EvalVar<size_t>(keyName+"OuterLineNBolt");  
  outerBoltStep=Control.EvalVar<double>(keyName+"OuterBoltStep");
  outerBoltRadius=Control.EvalVar<double>(keyName+"OuterBoltRadius");
  outerBoltMat=ModelSupport::EvalDefMat<int>(Control,keyName+"OuterBoltMat",0);
  
  boltMat=ModelSupport::EvalMat<int>(Control,keyName+"BoltMat");

  return;
}

void
TwinBase::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors.
    - Main is considered the actual build with Z in gravity direction
    - Beam as the beam direction (can be sloped)
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("TwinBase","createUnitVector");

  attachSystem::FixedComp& Main=getKey("Main");
  attachSystem::FixedComp& Beam=getKey("Beam");
  attachSystem::FixedComp& MotorA=getKey("MotorBase");
  attachSystem::FixedComp& MotorB=getKey("MotorTop");

  Beam.createUnitVector(FC,sideIndex);
  Main.createUnitVector(FC,sideIndex);
  MotorA.createUnitVector(FC,sideIndex);
  MotorB.createUnitVector(FC,sideIndex);

  //  Main.applyShift(0.0,0,0,beamZStep);

  applyOffset();  
  setDefault("Main");

  MotorA.applyShift(0,0,-innerLowStep);
  MotorB.applyShift(0,0,innerTopStep);

  lowOutCent=Origin-Z*(stepHeight/2.0);
  topOutCent=Origin+Z*(stepHeight/2.0);

  return;
}


void
TwinBase::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TwinBase","createSurfaces");

  const attachSystem::FixedComp& MotorA=getKey("MotorBase");
  const attachSystem::FixedComp& MotorB=getKey("MotorTop");

  ModelSupport::buildPlane(SMap,houseIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+3,Origin-X*mainRadius,X);
  ModelSupport::buildPlane(SMap,houseIndex+4,Origin+X*mainRadius,X);
  ModelSupport::buildPlane(SMap,houseIndex+5,Origin-Z*(stepHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,houseIndex+6,Origin+Z*(stepHeight/2.0),Z);

  ModelSupport::buildCylinder(SMap,houseIndex+7,lowOutCent,Y,mainRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+8,topOutCent,Y,mainRadius);

  // Bolt layer
  const double boltOffset(2.0*(outerBoltStep+outerBoltRadius));
  ModelSupport::buildPlane(SMap,houseIndex+23,
                           Origin-X*(mainRadius-boltOffset),X);
  ModelSupport::buildPlane(SMap,houseIndex+24,
                           Origin+X*(mainRadius-boltOffset),X);
  ModelSupport::buildCylinder(SMap,houseIndex+27,lowOutCent,Y,
                              mainRadius-boltOffset);
  ModelSupport::buildCylinder(SMap,houseIndex+28,topOutCent,Y,
			mainRadius-boltOffset);

  // Inner space
  ModelSupport::buildPlane(SMap,houseIndex+11,Origin-Y*(innerVoid/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+12,Origin+Y*(innerVoid/2.0),Y);

  ModelSupport::buildCylinder(SMap,houseIndex+17,MotorA.getCentre(),
			      Y,innerRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+18,MotorB.getCentre(),
			      Y,innerRadius);

  return;
}


void
TwinBase::createOuterBolts(Simulation& System,const int surfOffset,
			      const Geometry::Vec3D& Centre,
			      const std::string& FBStr,
			      const std::string& EdgeStr,
			      const double BRadius,const size_t NBolts,
			      const double radius,const double angOff,
			      const double arcAngle,const int startSurf,
                              const int endSurf)			      
  /*!
    Create the outer bolts
    \param System :: Simualation to use
    \param Centre :: Centre of dist
    \param FBStr :: Front/Back plates
    \param EdgeStr :: Edges of ring/area
    \param BRadius :: Radius of bolt from Centre
    \param NBolts :: Number of bolts
    \param radius :: radius of bolt
    \param angOff :: angle offset  [deg]
    \param arcAngle :: angular range of bolts (typcially 180/360) [deg]
    \param startSurf :: Start surface [signed]
    \param endSurf :: end surface [signed]
   */
{
  ELog::RegMethod RegA("TwinBase","createOuterBolts");


  std::string Out;
  if (outerRingNBolt>1)
    {
      const double angleR=arcAngle/static_cast<double>(NBolts);
      Geometry::Vec3D DPAxis(X);
      Geometry::Vec3D BAxis(Z*BRadius);
      const Geometry::Quaternion QStartSeg=
        Geometry::Quaternion::calcQRotDeg(angOff,Y);
      const Geometry::Quaternion QHalfSeg=
        Geometry::Quaternion::calcQRotDeg(angleR/2.0,Y);
      const Geometry::Quaternion QSeg=
        Geometry::Quaternion::calcQRotDeg(angleR,Y);
      
      // half a segment rotation to start:
      QStartSeg.rotate(DPAxis);
      QStartSeg.rotate(BAxis);
      QHalfSeg.rotate(BAxis);

      
      int boltIndex(surfOffset);
      SMap.addMatch(boltIndex+3,-startSurf);
      Geometry::Vec3D boltC=Centre+BAxis;
            
      ModelSupport::buildCylinder(SMap,boltIndex+7,boltC,Y,radius);
      boltIndex+=10;
      for(size_t i=1;i<NBolts;i++)
        {
          QSeg.rotate(DPAxis);
          QSeg.rotate(BAxis);
          boltC=Centre+BAxis;
          
          ModelSupport::buildCylinder(SMap,boltIndex+7,boltC,Y,radius);
          ModelSupport::buildPlane(SMap,boltIndex+3,Centre,DPAxis);
          boltIndex+=10;
        }
      SMap.addMatch(boltIndex+3,endSurf);
           
      // reset
      boltIndex=surfOffset;
      for(size_t i=0;i<NBolts;i++)
        {
          Out=ModelSupport::getComposite(SMap,boltIndex," -7 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out+FBStr));
          addCell("OuterBolts",cellIndex-1);
          
          Out=ModelSupport::getComposite(SMap,boltIndex," 3 -13 7 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,
                                           Out+FBStr+EdgeStr));
	  addCell("OuterWall",cellIndex-1);
          boltIndex+=10;
       }
    }
  else
    {
      // If here need fron / back angles
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,FBStr+EdgeStr));
      addCell("OuterWall",cellIndex-1);
    }

  return;
}

void
TwinBase::createLineBolts(Simulation& System,const int surfOffset,
                             const std::string& FBStr,
                             const std::string& leftEdgeStr,
                             const std::string& rightEdgeStr,
                             const double upDownLength,
                             const size_t NBolts,const double radius,
                             const int startSurf,const int endSurf)
  /*!
    Create the outer bolts
    \param System :: Simualation to use
    \param FBStr :: Front/Back plates
    \param leftEdgeStr :: left Edges of ring/area
    \param rightEdgeStr :: left Edges of ring/area
    \param upDownLength :: lenght of bolts
    \param NBolts :: Number of bolts
    \param radius :: radius of bolt
    \param startSurf :: Start surface [signed]
    \param endSurf :: end surface [signed]
   */
{
  ELog::RegMethod RegA("TwinBase","createLineBolts");

  std::string Out;

  if (NBolts>=1)
    {
      const double bStep(upDownLength/static_cast<double>(NBolts+1));
      int boltIndex(surfOffset);

      Geometry::Vec3D leftBoltCentre=
        Origin-X*(mainRadius-outerBoltStep-radius)-Z*(upDownLength/2.0);
      Geometry::Vec3D rightBoltCentre=
        Origin+X*(mainRadius-outerBoltStep-radius)-Z*(upDownLength/2.0);
      Geometry::Vec3D pCentre=Origin-Z*(upDownLength/2.0);

      leftBoltCentre+=Z*(bStep/2.0);
      rightBoltCentre+=Z*(bStep/2.0);

      ModelSupport::buildCylinder(SMap,boltIndex+7,leftBoltCentre,Y,radius);
      ModelSupport::buildCylinder(SMap,boltIndex+8,rightBoltCentre,Y,radius);
      SMap.addMatch(boltIndex+5,startSurf);
      boltIndex+=10;
      for(size_t i=1;i<NBolts;i++)
        {
          pCentre+=Z*bStep;
          leftBoltCentre+=Z*bStep;
          rightBoltCentre+=Z*bStep;
          ModelSupport::buildCylinder(SMap,boltIndex+7,leftBoltCentre,Y,radius);
          ModelSupport::buildCylinder(SMap,boltIndex+8,rightBoltCentre,Y,radius);
          ModelSupport::buildPlane(SMap,boltIndex+5,pCentre,Z);
          boltIndex+=10;
        }
      SMap.addMatch(boltIndex+5,endSurf);
           
      // reset
      boltIndex=surfOffset;
      for(size_t i=0;i<NBolts;i++)
        {
          Out=ModelSupport::getComposite(SMap,boltIndex," -7 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out+FBStr));
          addCell("OuterBolts",cellIndex-1);

          Out=ModelSupport::getComposite(SMap,boltIndex," -8 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out+FBStr));
          addCell("OuterBolts",cellIndex-1);
          
          Out=ModelSupport::getComposite(SMap,boltIndex," 5 -15  7 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+FBStr+leftEdgeStr));
	  addCell("OuterWall",cellIndex-1);
          
          Out=ModelSupport::getComposite(SMap,boltIndex," 5 -15 8 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+FBStr+rightEdgeStr));
	  addCell("OuterWall",cellIndex-1);
          boltIndex+=10;
       }
    }
  else
    {
      // If here need fron / back angles
      //      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,FBStr+EdgeStr));
      //      addCell("OuterWall",cellIndex-1);
    }

  return;
}
  
  
void
TwinBase::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TwinBase","createObjects");

  std::string Out,FBStr,EdgeStr,SealStr;

  // Main void
  Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 (-17:-18)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);
  
  // Main casing [inside bolt layer]
  Out=ModelSupport::getComposite
    (SMap,houseIndex,"1 -11 23 -24 (5:-27) (-6:-28) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("FrontCase",cellIndex-1);
  addCell("Case",cellIndex-1);

  Out=ModelSupport::getComposite
    (SMap,houseIndex,"12 -2 23 -24 (5:-27) (-6:-28)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("BackCase",cellIndex-1);
  addCell("Case",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,houseIndex,
                                 "11 -12 23 -24 (5:-27) (-6:-28) 17 18 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("EdgeCase",cellIndex-1);

  // OUTER RING :
  // NECESSARY because segment cut
  EdgeStr=ModelSupport::getComposite(SMap,houseIndex," -7 27 ");  
  FBStr=ModelSupport::getComposite(SMap,houseIndex," 1 -2 ");
  int divideSurf(SMap.realSurf(houseIndex+5));
  createOuterBolts(System,houseIndex+5000,lowOutCent,FBStr,EdgeStr,
                   mainRadius-(outerBoltStep+outerBoltRadius),
                   outerRingNBolt,outerBoltRadius,
                   90.0,180.0,divideSurf,divideSurf);

  EdgeStr=ModelSupport::getComposite(SMap,houseIndex," -8 28 ");
  divideSurf=SMap.realSurf(houseIndex+6);
  createOuterBolts(System,houseIndex+5500,topOutCent,FBStr,EdgeStr,
                   mainRadius-(outerBoltStep+outerBoltRadius),
                   outerRingNBolt,outerBoltRadius,
                   -90.0,180.0,-divideSurf,-divideSurf);

  const int lowCutSurf=SMap.realSurf(houseIndex+5);
  const int topCutSurf=SMap.realSurf(houseIndex+6);

  std::string leftEdgeStr=
    ModelSupport::getComposite(SMap,houseIndex," 3 -23 ");
  std::string rightEdgeStr=
    ModelSupport::getComposite(SMap,houseIndex," 24 -4 ");
  createLineBolts(System,houseIndex+6000,
                  FBStr,leftEdgeStr,rightEdgeStr,
                  stepHeight,outerLineNBolt,
                  outerBoltRadius,
                  lowCutSurf,topCutSurf);

  
  // Outer
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -2 3 -4 (5:-7) (-6:-8) ");
  addOuterSurf(Out);  
   
  return;
}

void
TwinBase::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("TwinBase","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  attachSystem::FixedComp& motorAFC=FixedGroup::getKey("MotorTop");
  attachSystem::FixedComp& motorBFC=FixedGroup::getKey("MotorBase");

  mainFC.setConnect(0,Origin-Y*(length/2.0),-Y);
  mainFC.setConnect(1,Origin+Y*(length/2.0),Y);
  mainFC.setConnect(2,Origin-X*mainRadius,-X);
  mainFC.setConnect(3,Origin+X*mainRadius,X);
  mainFC.setConnect(4,Origin-Z*(mainRadius+stepHeight/2.0),-Z);
  mainFC.setConnect(4,Origin+Z*(mainRadius+stepHeight/2.0),Z);

  mainFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  mainFC.setLinkSurf(2,-SMap.realSurf(houseIndex+3));
  mainFC.setLinkSurf(3,SMap.realSurf(houseIndex+4));
  mainFC.setLinkSurf(4,SMap.realSurf(houseIndex+7));
  mainFC.setLinkSurf(5,SMap.realSurf(houseIndex+8));

  // These are protected from ZVertial re-orientation
  const Geometry::Vec3D BC(beamFC.getCentre());
  const Geometry::Vec3D BY(beamFC.getY());
  
  beamFC.setConnect(0,BC-BY*(length/2.0),-BY);
  beamFC.setConnect(1,BC+BY*(length/2.0),BY);

  beamFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));

  const Geometry::Vec3D& LC=motorAFC.getCentre();
  motorAFC.setConnect(0,LC-Y*(length/2.0),-Y);
  motorAFC.setConnect(1,LC+Y*(length/2.0),Y);
  motorAFC.setConnect(2,LC,Y);
  
  motorAFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  motorAFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  motorAFC.setLinkSurf(2,SMap.realSurf(houseIndex+2));

  const Geometry::Vec3D& TC=motorAFC.getCentre();
  motorBFC.setConnect(0,TC-Y*(length/2.0),-Y);
  motorBFC.setConnect(1,TC+Y*(length/2.0),Y);
  motorBFC.setConnect(2,TC,Y);
  
  motorBFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  motorBFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  motorBFC.setLinkSurf(2,SMap.realSurf(houseIndex+2));




  return;
}

void
TwinBase::insertAxle(Simulation& System,
			 const attachSystem::CellMap& CMlow,
			 const attachSystem::CellMap& CMtop) const
  /*!
    Accessor function to allow the axle to be put in the 
    disks
    \param System :: Simulation to use
    \param CMlow :: lower Cell Map to get disks from -- requires
    named cells (Inner).
    \param CMtop :: upper Cell Map to get disks from -- requires
    named cells (Inner).
  */
{
  ELog::RegMethod RegA("TwinBase","insertAxle");

  motorA->insertInCell("Axle",System,CMtop.getCells("Inner"));
  motorB->insertInCell("Axle",System,CMlow.getCells("Inner"));


  return;
}

void
TwinBase::createMotor(Simulation& System,
			  const std::string& posName,
			  std::shared_ptr<Motor>& motor)

  /*!
    Create the motor
    \param System :: Simulation to use
    \param posName :: Motor key side
    \param motor :: motor pointer
  */
{
  ELog::RegMethod RegA("TwinBase","createMotor");

  
  motor->addInsertCell("Plate",getCells("Case"));
  motor->addInsertCell("Axle",getCell("Void"));

  motor->setInnerPlanes(SMap.realSurf(houseIndex+11),
			-SMap.realSurf(houseIndex+12));
  motor->setYSteps(length/2.0,length/2.0);
  motor->createAll(System,FixedGroup::getKey(posName),0);

  addSurf(posName+"Axle",motor->getSurf("axle"));
  return;
}

void
TwinBase::processInsert(Simulation& System)
  /*!
    Insert motor and do the FULL insertion of the Twinchopper
    \param System :: simulation to use
  */
{
  ELog::RegMethod RegA("TwinBase","processInsert");
  
  motorA->addInsertCell("Outer",*this);
  motorB->addInsertCell("Outer",*this);
  
  insertObjects(System);   

  return;
}


void
TwinBase::buildMotors(Simulation& System)
  /*!
    Build the motors
    \param System :: simulation to use
  */
{
  ELog::RegMethod RegA("TwinBase","buildMotor");
  
  createMotor(System,"MotorTop",motorA);
  createMotor(System,"MotorBase",motorB); 

  return;
}
  
  
  
}  // NAMESPACE constructSystem
