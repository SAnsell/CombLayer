/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TwinChopper.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "RingSeal.h"
#include "InnerPort.h"
#include "TwinChopper.h"

namespace constructSystem
{

TwinChopper::TwinChopper(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",6,"BuildBeam",2,"Motor",6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  houseIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(houseIndex+1),
  RS(new constructSystem::RingSeal(Key+"Ring")),
  IPA(new constructSystem::InnerPort(Key+"IPortA")),
  IPB(new constructSystem::InnerPort(Key+"IPortB"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(RS);
  OR.addObject(IPA);
  OR.addObject(IPB);
}


TwinChopper::~TwinChopper() 
  /*!
    Destructor
  */
{}

void
TwinChopper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("TwinChopper","populate");

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

  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portOuter=Control.EvalVar<double>(keyName+"PortOuter");
  portStep=Control.EvalVar<double>(keyName+"PortStep");
  portBoltRad=Control.EvalVar<double>(keyName+"PortBoltRadius");
  portNBolt=Control.EvalVar<size_t>(keyName+"PortNBolt");
  portBoltAngOff=Control.EvalDefVar<double>(keyName+"PortBoltAngOff",0.0);
  portSeal=Control.EvalDefVar<double>(keyName+"PortSealThick",0.0);
  portSealMat=ModelSupport::EvalMat<int>(Control,keyName+"PortSealMat");

  motorAFlag=Control.EvalVar<int>(keyName+"MotorAFlag");
  motorARadius=Control.EvalVar<double>(keyName+"MotorARadius");
  motorAOuter=Control.EvalVar<double>(keyName+"MotorAOuter");
  motorAStep=Control.EvalVar<double>(keyName+"MotorAStep");
  motorANBolt=Control.EvalVar<size_t>(keyName+"MotorANBolt");
  motorABoltRad=Control.EvalVar<double>(keyName+"MotorABoltRadius");
  motorABoltAngOff=Control.EvalDefVar<double>(keyName+"MotorABoltAngOff",0.0);
  motorASeal=Control.EvalDefVar<double>(keyName+"MotorASealThick",0.0);
  motorASealMat=ModelSupport::EvalMat<int>(Control,keyName+"MotorASealMat");
  motorAMat=ModelSupport::EvalDefMat<int>(Control,keyName+"MotorAMat",0);

  motorBFlag=Control.EvalVar<int>(keyName+"MotorBFlag");  
  motorBRadius=Control.EvalVar<double>(keyName+"MotorBRadius");
  motorBOuter=Control.EvalVar<double>(keyName+"MotorBOuter");
  motorBStep=Control.EvalVar<double>(keyName+"MotorBStep");
  motorBNBolt=Control.EvalVar<size_t>(keyName+"MotorBNBolt");
  motorBBoltRad=Control.EvalVar<double>(keyName+"MotorBBoltRadius");
  motorBBoltAngOff=Control.EvalDefVar<double>(keyName+"MotorBBoltAngOff",0.0);
  motorBSeal=Control.EvalDefVar<double>(keyName+"MotorBSealThick",0.0);
  motorBSealMat=ModelSupport::EvalMat<int>(Control,keyName+"MotorBSealMat");
  motorBMat=ModelSupport::EvalDefMat<int>(Control,keyName+"MotorBMat",0);

  
  outerRingNBolt=Control.EvalVar<size_t>(keyName+"OuterRingNBolt");
  outerLineNBolt=Control.EvalVar<size_t>(keyName+"OuterLineNBolt");  
  outerBoltStep=Control.EvalVar<double>(keyName+"OuterBoltStep");
  outerBoltRadius=Control.EvalVar<double>(keyName+"OuterBoltRadius");
  outerBoltMat=ModelSupport::EvalDefMat<int>(Control,keyName+"MotorBMat",0);
  
  boltMat=ModelSupport::EvalMat<int>(Control,keyName+"BoltMat");

  return;
}

void
TwinChopper::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors.
    - Main is considered the actual build with Z in gravity direction
    - Beam as the beam direction (can be sloped)
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("TwinChopper","createUnitVector");

  attachSystem::FixedComp& Main=getKey("Main");
  attachSystem::FixedComp& Beam=getKey("BuildBeam");
  attachSystem::FixedComp& Motor=getKey("Motor");

  Beam.createUnitVector(FC,sideIndex);
  Main.createUnitVector(FC,sideIndex);
  Motor.createUnitVector(FC,sideIndex);

  //  Main.applyShift(0.0,0,0,beamZStep);

  applyOffset();  
  setDefault("Main");

  lowCentre=Origin-Z*innerLowStep;
  topCentre=Origin+Z*innerTopStep;

  lowOutCent=Origin-Z*(stepHeight/2.0);
  topOutCent=Origin+Z*(stepHeight/2.0);

  return;
}


void
TwinChopper::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("TwinChopper","createSurfaces");

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

  ModelSupport::buildCylinder(SMap,houseIndex+17,lowCentre,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+18,topCentre,Y,innerRadius);
  
  // MOTORS [3000/4000]
  ModelSupport::buildCylinder(SMap,houseIndex+3007,lowCentre,Y,motorARadius);
  ModelSupport::buildCylinder(SMap,houseIndex+3017,lowCentre,Y,motorAOuter);

  ModelSupport::buildCylinder(SMap,houseIndex+4007,topCentre,Y,motorBRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+4017,topCentre,Y,motorBOuter);


  
  // Construct beamport:
  setDefault("BuildBeam");
  ModelSupport::buildCylinder(SMap,houseIndex+2007,Origin,Y,portRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+2017,Origin,Y,portOuter);

  if (portSeal>Geometry::zeroTol)
    {
      const double sealYDist((innerVoid+length)/4.0);
      
      ModelSupport::buildCylinder(SMap,houseIndex+2008,
                                  Origin,Y,portOuter-2.0*portSeal);
      ModelSupport::buildCylinder(SMap,houseIndex+2018,
                                  Origin,Y,portOuter-portSeal);
      
      ModelSupport::buildPlane(SMap,houseIndex+2001,
                               Origin-Y*(sealYDist+portSeal/2.0),Y);
      ModelSupport::buildPlane(SMap,houseIndex+2002,
                               Origin-Y*(sealYDist-portSeal/2.0),Y);
      ModelSupport::buildPlane(SMap,houseIndex+2011,
                               Origin+Y*(sealYDist-portSeal/2.0),Y);
      ModelSupport::buildPlane(SMap,houseIndex+2012,
                               Origin+Y*(sealYDist+portSeal/2.0),Y);
    }
  return;
  // MAIN OUTER BOLTS AT [5000]
  int BI(houseIndex+5000);

  double angle(0.0);
  const double angStep(M_PI/(outerRingNBolt+1));
  const Geometry::Vec3D RX(X*(mainRadius-outerBoltStep));
  const Geometry::Vec3D RZ(Z*(mainRadius-outerBoltStep));
  for(size_t i=0;i<outerRingNBolt;i++)
    {
      angle+=angStep;
      ModelSupport::buildCylinder
	(SMap,BI+7,lowOutCent-RX*cos(angle)-RZ*sin(angle),Y,outerBoltRadius);
      ModelSupport::buildCylinder
	(SMap,BI+8,topOutCent-RX*cos(angle)+RZ*sin(angle),Y,outerBoltRadius);
      BI+=10;
    }
  // MAIN Line bolts
  BI=houseIndex+6000;
  Geometry::Vec3D zPos(lowOutCent);
  const Geometry::Vec3D ZStep((topOutCent-lowOutCent)/(outerLineNBolt+1.0));
  
  for(size_t i=0;i<outerLineNBolt;i++)
    {
      zPos+=ZStep;
      ModelSupport::buildCylinder(SMap,BI+7,zPos-RX,Y,outerBoltRadius);
      ModelSupport::buildCylinder(SMap,BI+8,zPos+RX,Y,outerBoltRadius);
      BI+=10;      
    }
  return;
}

void
TwinChopper::createRing(Simulation& System,const int surfOffset,
                        const Geometry::Vec3D& Centre,
                        const std::string& FBStr,
                        const std::string& EdgeStr,
                        const double BRad,const size_t NBolts,
                        const double radius,const double angOff,
			const std::string& sealUnit,const int sealMat)
  /*!
    Create the ring of bolts : 
    Only works if NBolts != 1.
    Assumes that surface surfOffset+7/17 have been created
    
    \param System :: Simulation
    \param surfOffset :: Start of surface offset
    \param Centre :: Centre of dist
    \param EdgeStr :: Edges of ring/area
    \param FBStr :: Front/Back plates
    \param BRad :: Radius of bolt from Centre
    \param NBolts :: Number of bolts
    \param radius :: radius of bolt
    \param angOff :: angle offset
    \param sealUnit :: string for the seal unit
    \param sealMat :: material for the seal
  */
{
  ELog::RegMethod RegA("TwinChopper","createRing");
   // Construct surfaces:

  const bool sealFlag(!sealUnit.empty());
  std::string sealUnitComp;
  if (sealFlag)
    {
      HeadRule SComp(sealUnit);
      SComp.makeComplement();
      sealUnitComp=SComp.display();
    }  
  std::string Out;
  if (NBolts>1)
    {
      const double angleR=360.0/static_cast<double>(NBolts);
      Geometry::Vec3D DPAxis(X);
      Geometry::Vec3D BAxis(Z*BRad);
      const Geometry::Quaternion QStartSeg=
        Geometry::Quaternion::calcQRotDeg(angOff,Y);
      const Geometry::Quaternion QHalfSeg=
        Geometry::Quaternion::calcQRotDeg(angleR/2.0,Y);
      const Geometry::Quaternion QSeg=
        Geometry::Quaternion::calcQRotDeg(angleR,Y);
      
      // half a segment rotation to start:
      QStartSeg.rotate(DPAxis);
      QStartSeg.rotate(BAxis);
      QHalfSeg.rotate(DPAxis);
      
      int boltIndex(surfOffset+100);
      for(size_t i=0;i<NBolts;i++)
        {
          const Geometry::Vec3D boltC(Centre+BAxis);
          
          ModelSupport::buildCylinder(SMap,boltIndex+7,boltC,Y,radius);
          ModelSupport::buildPlane(SMap,boltIndex+3,Centre,DPAxis);
          QSeg.rotate(DPAxis);
          QSeg.rotate(BAxis);
          boltIndex+=10;
        }
      
      // reset
      int prevBoltIndex(boltIndex-10);
      boltIndex=surfOffset+100;
      for(size_t i=0;i<NBolts;i++)
        {
          Out=ModelSupport::getComposite(SMap,boltIndex," -7 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out+FBStr));
          addCell("Bolts",cellIndex-1);
          
          Out=ModelSupport::getComposite(SMap,prevBoltIndex,boltIndex,
                                         " 3  -3M 7M ");
          
          System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,
                                           Out+FBStr+EdgeStr+sealUnitComp));
	  addCell("Wall",cellIndex-1);
	  if (sealFlag)
	    {
	      Out=ModelSupport::getComposite(SMap,prevBoltIndex,boltIndex,
					     " 3 -3M ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,sealMat,0.0,
					       Out+sealUnit));
	      addCell("Seal",cellIndex-1);
	    }
	  prevBoltIndex=boltIndex;
          boltIndex+=10;
       }
    }
  else
    {
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,FBStr+EdgeStr));
      addCell("Wall",cellIndex-1);
    }
  return;
}

std::string
TwinChopper::motorFrontExclude() const
  /*!
    Simple function to get motor cut flag if motor is front/back
    \return exlude string [not corrected]
  */
{
  std::string Out;
  if (motorAFlag & 1) Out+=" 3017 ";
  if (motorBFlag & 1) Out+=" 4017 ";
  return Out;
}

std::string
TwinChopper::motorBackExclude() const
  /*!
    Simple function to get motor cut flag if motor is back
    \return exlude string [not corrected]
  */
{
  std::string Out;
  if (motorAFlag & 2) Out+=" 3017 ";
  if (motorBFlag & 2) Out+=" 4017 ";
  return Out;
}


void
TwinChopper::createOuterBolts(Simulation& System,const int surfOffset,
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
    \param EdgeStr :: Edges of ring/area
    \param FBStr :: Front/Back plates
    \param BRadius :: Radius of bolt from Centre
    \param NBolts :: Number of bolts
    \param radius :: radius of bolt
    \param angOff :: angle offset
    \param startSurf :: Start surface [signed]
    \param endSurf :: end surface [signed]
   */
{
  ELog::RegMethod RegA("TwinChopper","createOuterBolts");


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
TwinChopper::createLineBolts(Simulation& System,const int surfOffset,
                             const std::string& FBStr,
                             const std::string& leftEdgeStr,
                             const std::string& rightEdgeStr,
                             const double upDownLength,
                             const size_t NBolts,const double radius,
                             const int startSurf,const int endSurf)
  /*!
    Create the outer bolts
    \param System :: Simualation to use
    \param EdgeStr :: Edges of ring/area
    \param FBStr :: Front/Back plates
    \param upDownLength :: lenght of bolts
    \param NBolts :: Number of bolts
    \param radius :: radius of bolt
    \param angOff :: angle offset
    \param startSurf :: Start surface [signed]
    \param endSurf :: end surface [signed]
   */
{
  ELog::RegMethod RegA("TwinChopper","createLineBolts");


  std::string Out;

  if (NBolts>=1)
    {
      const double bStep(upDownLength/(NBolts+1));
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
TwinChopper::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    */
{
  ELog::RegMethod RegA("TwinChopper","createObjects");

  const attachSystem::FixedComp& Main=getKey("Main");
  const attachSystem::FixedComp& Beam=getKey("BuildBeam");
  const double CentreDist=Main.getCentre().Distance(Beam.getCentre());
  
  std::string Out,FBStr,EdgeStr,SealStr;

  // Main void
  Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 (-17:-18)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Port [Front/back]
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -2007");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("PortVoid",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -2007");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("PortVoid",cellIndex-1);

  
  // Main casing [inside bolt layer]
  Out=ModelSupport::getComposite(SMap,houseIndex,
                                 "1 -11 23 -24 (5:-27) (-6:-28) 2017 "+
				 motorFrontExclude());
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Case",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,houseIndex,
                                 "12 -2 23 -24 (5:-27) (-6:-28) 2017 "+
				 motorBackExclude());
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Case",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,houseIndex,
                                 "11 -12 23 -24 (5:-27) (-6:-28) 17 18 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Case",cellIndex-1);

  
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -2007");
  IPA->addInnerCell(getCell("PortVoid",0));
  IPA->createAll(System,Beam,0,Out);

    // Add inner system

  Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -2007");
  IPB->addInnerCell(getCell("PortVoid",1));
  ELog::EM<<"Inner Cell = "<<getCell("PortVoid",1)<<ELog::endDiag;
  IPB->createAll(System,Beam,0,Out);

  // Front ring seal
  FBStr=ModelSupport::getComposite(SMap,houseIndex," 1 -11 ");
  EdgeStr=ModelSupport::getComposite(SMap,houseIndex+2000," 7 -17 ");
  SealStr=ModelSupport::getComposite(SMap,houseIndex+2000," 8 -18 1 -2 ");
  createRing(System,houseIndex+2000,Beam.getCentre(),FBStr,EdgeStr,
             (portRadius+portOuter)/2.0,portNBolt,portBoltRad,
             portBoltAngOff,SealStr,portSealMat);
  
  // back ring seal
  FBStr=ModelSupport::getComposite(SMap,houseIndex," 12 -2 ");
  SealStr=ModelSupport::getComposite(SMap,houseIndex+2000," 8 -18 11 -12 ");
  createRing(System,houseIndex+2500,Beam.getCentre(),FBStr,EdgeStr,
             (portRadius+portOuter)/2.0,portNBolt,portBoltRad,
             portBoltAngOff,SealStr,portSealMat);
  
  // Extra wall for bolts
  if (CentreDist+portOuter>mainRadius)
    {
      ELog::EM<<"Extension material: "<<cellIndex<<ELog::endDiag;
      Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 17 -2017");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      addCell("Wall",cellIndex-1);
    }
  
  // MOTOR:
  // [front/back] of LOWER
  EdgeStr=ModelSupport::getComposite(SMap,houseIndex+3000," 7 -17 ");
  if (motorAFlag & 1)
    {
      Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -3007");
      System.addCell(MonteCarlo::Qhull(cellIndex++,motorAMat,0.0,Out));
      addCell("MotorVoid",cellIndex-1);
      // Divide surfaces and bolts
      FBStr=ModelSupport::getComposite(SMap,houseIndex," 1 -11 ");
      createRing(System,houseIndex+3000,lowCentre,FBStr,EdgeStr,
		 (motorARadius+motorAOuter)/2.0,motorANBolt,motorABoltRad,
		 motorABoltAngOff,"",0);
    }
  
  if (motorAFlag & 2)
    {
      Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -3007"); 
      System.addCell(MonteCarlo::Qhull(cellIndex++,motorAMat,0.0,Out));
      addCell("MotorVoid",cellIndex-1);
      FBStr=ModelSupport::getComposite(SMap,houseIndex," 12 -2 ");
      createRing(System,houseIndex+3500,lowCentre,FBStr,EdgeStr,
		 (motorARadius+motorAOuter)/2.0,motorANBolt,motorABoltRad,
		 motorABoltAngOff,"",0);
    }

  // MOTOR:
  // [front/back] of TOP
  EdgeStr=ModelSupport::getComposite(SMap,houseIndex+4000," 7 -17 ");
  if (motorBFlag & 1)
    {
      Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -4007");
      System.addCell(MonteCarlo::Qhull(cellIndex++,motorBMat,0.0,Out));
      addCell("MotorVoid",cellIndex-1);
      // Divide surfaces and bolts
      FBStr=ModelSupport::getComposite(SMap,houseIndex," 1 -11 ");
      
      createRing(System,houseIndex+4000,topCentre,FBStr,EdgeStr,
		 (motorBRadius+motorBOuter)/2.0,motorBNBolt,motorBBoltRad,
		 motorBBoltAngOff,"",0);
    }
  if (motorBFlag & 2)
    {
      Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -4007"); 
      System.addCell(MonteCarlo::Qhull(cellIndex++,motorBMat,0.0,Out));
      addCell("MotorVoid",cellIndex-1);

  
      FBStr=ModelSupport::getComposite(SMap,houseIndex," 12 -2 ");
      createRing(System,houseIndex+4500,topCentre,FBStr,EdgeStr,
		 (motorBRadius+motorBOuter)/2.0,motorBNBolt,motorBBoltRad,
             motorBBoltAngOff,"",0);
    }

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
TwinChopper::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("TwinChopper","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("BuildBeam");
  attachSystem::FixedComp& motorFC=FixedGroup::getKey("Motor");

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

  motorFC.setConnect(0,lowCentre-Y*(length/2.0),-Y);
  motorFC.setConnect(1,lowCentre+Y*(length/2.0),Y);
  motorFC.setConnect(2,lowCentre,Y);
  motorFC.setConnect(3,topCentre+Y*(length/2.0),Y);
  motorFC.setConnect(4,topCentre-Y*(length/2.0),-Y);
  motorFC.setConnect(5,topCentre,Y);
  
  motorFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  motorFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  motorFC.setLinkSurf(3,-SMap.realSurf(houseIndex+1));
  motorFC.setLinkSurf(4,SMap.realSurf(houseIndex+2));



  return;
}

void
TwinChopper::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("TwinChopper","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(beamFC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  //  RS->createAll(System,FixedGroup::getKey("Main"),0);

  return;
}
  
}  // NAMESPACE constructSystem
