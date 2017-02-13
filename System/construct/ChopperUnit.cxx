/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/ChopperUnit.cxx
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
#include "ChopperUnit.h"

namespace constructSystem
{

ChopperUnit::ChopperUnit(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2,"BuildBeam",0),
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

ChopperUnit::ChopperUnit(const ChopperUnit& A) : 
  attachSystem::FixedOffsetGroup(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  houseIndex(A.houseIndex),cellIndex(A.cellIndex),
  height(A.height),
  width(A.width),depth(A.depth),length(A.length),
  shortHeight(A.shortHeight),shortWidth(A.shortWidth),
  mainRadius(A.mainRadius),mainThick(A.mainThick),
  motorRadius(A.motorRadius),motorOuter(A.motorOuter),
  motorStep(A.motorStep),portRadius(A.portRadius),
  portOuter(A.portOuter),portStep(A.portStep),
  portWindow(A.portWindow),portNBolt(A.portNBolt),
  portBoltRad(A.portBoltRad),portBoltAngOff(A.portBoltAngOff),
  portSeal(A.portSeal),portSealMat(A.portSealMat),
  portWindowMat(A.portWindowMat),motorNBolt(A.motorNBolt),
  motorBoltRad(A.motorBoltRad),motorBoltAngOff(A.motorBoltAngOff),
  motorSeal(A.motorSeal),motorSealMat(A.motorSealMat),
  motorMat(A.motorMat),boltMat(A.boltMat),wallMat(A.wallMat),
  RS(new RingSeal(*A.RS)),IPA(new InnerPort(*A.IPA)),
  IPB(new InnerPort(*A.IPB))
  /*!
    Copy constructor
    \param A :: ChopperUnit to copy
  */
{}

ChopperUnit&
ChopperUnit::operator=(const ChopperUnit& A)
  /*!
    Assignment operator
    \param A :: ChopperUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      height=A.height;
      width=A.width;
      depth=A.depth;
      length=A.length;
      shortHeight=A.shortHeight;
      shortWidth=A.shortWidth;
      mainRadius=A.mainRadius;
      mainThick=A.mainThick;
      motorRadius=A.motorRadius;
      motorOuter=A.motorOuter;
      motorStep=A.motorStep;
      portRadius=A.portRadius;
      portOuter=A.portOuter;
      portStep=A.portStep;
      portWindow=A.portWindow;
      portNBolt=A.portNBolt;
      portBoltRad=A.portBoltRad;
      portBoltAngOff=A.portBoltAngOff;
      portSeal=A.portSeal;
      portSealMat=A.portSealMat;
      portWindowMat=A.portWindowMat;
      motorNBolt=A.motorNBolt;
      motorBoltRad=A.motorBoltRad;
      motorBoltAngOff=A.motorBoltAngOff;
      motorSeal=A.motorSeal;
      motorSealMat=A.motorSealMat;
      motorMat=A.motorMat;
      boltMat=A.boltMat;
      wallMat=A.wallMat;
      *RS=*A.RS;
      *IPA=*A.IPA;
      *IPB=*A.IPB;
    }
  return *this;
}

ChopperUnit::~ChopperUnit() 
  /*!
    Destructor
  */
{}

void
ChopperUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ChopperUnit","populate");

  FixedOffsetGroup::populate(Control);
  //  + Fe special:
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");
  shortWidth=Control.EvalVar<double>(keyName+"ShortWidth");
  const double diff(width-shortWidth);
  shortHeight=Control.EvalDefVar<double>(keyName+"ShortHeight",height-diff);

  mainRadius=Control.EvalVar<double>(keyName+"MainRadius");
  mainThick=Control.EvalVar<double>(keyName+"MainThick");

  motorRadius=Control.EvalVar<double>(keyName+"MotorRadius");
  motorOuter=Control.EvalVar<double>(keyName+"MotorOuter");
  motorStep=Control.EvalVar<double>(keyName+"MotorStep");
  motorNBolt=Control.EvalVar<size_t>(keyName+"MotorNBolt");
  motorBoltRad=Control.EvalVar<double>(keyName+"MotorBoltRadius");
  
  motorBoltAngOff=Control.EvalDefVar<double>(keyName+"MotorBoltAngOff",0.0);
  motorSeal=Control.EvalDefVar<double>(keyName+"MotorSealThick",0.0);
  motorSealMat=ModelSupport::EvalMat<int>(Control,keyName+"PortSealMat");
  motorMat=ModelSupport::EvalDefMat<int>(Control,keyName+"MotorMat",0);
  
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portOuter=Control.EvalVar<double>(keyName+"PortOuter");
  portStep=Control.EvalVar<double>(keyName+"PortStep");
  portBoltRad=Control.EvalVar<double>(keyName+"PortBoltRadius");
  portNBolt=Control.EvalVar<size_t>(keyName+"PortNBolt");
  portBoltAngOff=Control.EvalDefVar<double>(keyName+"PortBoltAngOff",0.0);
  portSeal=Control.EvalDefVar<double>(keyName+"PortSealThick",0.0);
  portSealMat=ModelSupport::EvalMat<int>(Control,keyName+"PortSealMat");
  
  boltMat=ModelSupport::EvalMat<int>(Control,keyName+"BoltMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
ChopperUnit::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("ChopperUnit","createUnitVector");

  attachSystem::FixedComp& Main=getKey("Main");
  attachSystem::FixedComp& BuildBeam=getKey("BuildBeam");
  attachSystem::FixedComp& Beam=getKey("Beam");

  Beam.createUnitVector(FC,sideIndex);
  BuildBeam.createUnitVector(FC,sideIndex);
  Main.createUnitVector(FC,sideIndex);

  applyOffset();
  setDefault("Main");
  return;
}


void
ChopperUnit::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("ChopperUnit","createSurfaces");

  const Geometry::Vec3D XLong=X*(width/2.0);
  const Geometry::Vec3D ZLong=Z*(height/2.0);
  const Geometry::Vec3D XShort=X*(shortWidth/2.0);
  const Geometry::Vec3D ZShort=Z*(shortHeight/2.0);

  ModelSupport::buildPlane(SMap,houseIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+3,Origin-XLong,X);
  ModelSupport::buildPlane(SMap,houseIndex+4,Origin+XLong,X);
  ModelSupport::buildPlane(SMap,houseIndex+5,Origin-ZLong,Z);
  ModelSupport::buildPlane(SMap,houseIndex+6,Origin+ZLong,Z);
      // triangular corners

      // All triangle edges point to centre:
  ModelSupport::buildPlane(SMap,houseIndex+7,
                           Origin-XLong-ZShort,
                           Origin-XShort-ZLong,
                           Origin-XShort-ZLong+Y,
                           Z);
  ModelSupport::buildPlane(SMap,houseIndex+8,
                           Origin-XLong+ZShort,
                           Origin-XShort+ZLong,
                           Origin-XShort+ZLong+Y,
                           -Z);
  ModelSupport::buildPlane(SMap,houseIndex+9,
                           Origin+XLong+ZShort,
                           Origin+XShort+ZLong,
                           Origin+XShort+ZLong+Y,
                           -Z);
  ModelSupport::buildPlane(SMap,houseIndex+10,
                           Origin+XLong-ZShort,
                           Origin+XShort-ZLong,
                           Origin+XShort-ZLong+Y,
                           Z);


  // CREATE INNER VOID:
  ModelSupport::buildPlane(SMap,houseIndex+11,Origin-Y*(mainThick/2.0),Y);
  ModelSupport::buildPlane(SMap,houseIndex+12,Origin+Y*(mainThick/2.0),Y);
  ModelSupport::buildCylinder(SMap,houseIndex+17,Origin,Y,mainRadius);

  // MOTOR [1000]
  ModelSupport::buildCylinder(SMap,houseIndex+1007,Origin,Y,motorRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+1017,Origin,Y,motorOuter);

  // Construct beamport:
  setDefault("BuildBeam");
  ModelSupport::buildCylinder(SMap,houseIndex+2007,Origin,Y,portRadius);
  ModelSupport::buildCylinder(SMap,houseIndex+2017,Origin,Y,portOuter);

  if (portSeal>Geometry::zeroTol)
    {
      const double sealYDist((mainThick+length)/4.0);
      
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
}

void
ChopperUnit::createRing(Simulation& System,const int surfOffset,
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
    \param centre :: Centre of dist
    \param EdgeStr :: Edges of ring/area
    \param FBStr :: Front/Back plates
    \param BRad :: Bolt Rad
    \param NBolts :: Number of bolts
    \param radius :: radius of bolt
    \param angOff :: angle offset
    \param sealUnit :: string for the seal unit
    \param sealMat :: material for the seal
  */
{
  ELog::RegMethod RegA("ChopperUnit","createRing");
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
  
void
ChopperUnit::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    */
{
  ELog::RegMethod RegA("ChopperUnit","createObjects");

  const attachSystem::FixedComp& Main=getKey("Main");
  const attachSystem::FixedComp& Beam=getKey("BuildBeam");
  const double CentreDist=Main.getCentre().Distance(Beam.getCentre());
  
  std::string Out,FBStr,EdgeStr,SealStr;

    // Main void
  Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 -17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Port [Front/back]
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -2007");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("PortVoid",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -2007");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("PortVoid",cellIndex-1);

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

  // If needed create the port material unit in the port
  if (CentreDist+portOuter>mainRadius)
    {
      Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 17 -2017");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      addCell("Wall",cellIndex-1);
    }

  // Motor [front/back]
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -1007");
  System.addCell(MonteCarlo::Qhull(cellIndex++,motorMat,0.0,Out));
  addCell("MotorVoid",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -1007"); 
  System.addCell(MonteCarlo::Qhull(cellIndex++,motorMat,0.0,Out));
  addCell("MotorVoid",cellIndex-1);

  // Divide surfaces
  FBStr=ModelSupport::getComposite(SMap,houseIndex," 1 -11 ");
  EdgeStr=ModelSupport::getComposite(SMap,houseIndex+1000," 7 -17 ");
  createRing(System,houseIndex+1000,Main.getCentre(),FBStr,EdgeStr,
             (motorRadius+motorOuter)/2.0,motorNBolt,motorBoltRad,
             motorBoltAngOff,"",0);
  
  
  FBStr=ModelSupport::getComposite(SMap,houseIndex," 12 -2 ");
  createRing(System,houseIndex+1500,Main.getCentre(),FBStr,EdgeStr,
             (motorRadius+motorOuter)/2.0,motorNBolt,motorBoltRad,
             motorBoltAngOff,"",0);

  // Main block
  Out=ModelSupport::getComposite(SMap,houseIndex,
                "1 -2 3 -4 5 -6 7 8 9 10 1017 2017 (-11:12:17)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Wall",cellIndex-1);

  // Add inner system

  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -11 -2007");
  IPA->addInnerCell(getCell("PortVoid",0));
  IPA->createAll(System,FixedGroup::getKey("BuildBeam"),0,Out);

  Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 -2007");
  IPB->addInnerCell(getCell("PortVoid",1));
  IPB->createAll(System,FixedGroup::getKey("BuildBeam"),0,Out);
  
  // Outer
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -2 3 -4 5 -6 7 8 9 10");
  addOuterSurf(Out);  
  
  return;
}

void
ChopperUnit::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ChopperUnit","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.setConnect(0,Origin-Y*(length/2.0),-Y);
  mainFC.setConnect(1,Origin+Y*(length/2.0),Y);
  mainFC.setConnect(2,Origin-X*(width/2.0),-X);
  mainFC.setConnect(3,Origin+X*(width/2.0),X);
  mainFC.setConnect(4,Origin-Z*(height/2.0),-Z);
  mainFC.setConnect(5,Origin-Z*(height/2.0),Z);

  mainFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  mainFC.setLinkSurf(2,-SMap.realSurf(houseIndex+3));
  mainFC.setLinkSurf(3,SMap.realSurf(houseIndex+4));
  mainFC.setLinkSurf(4,-SMap.realSurf(houseIndex+5));
  mainFC.setLinkSurf(5,SMap.realSurf(houseIndex+6));

  // These are protected from ZVertial re-orientation
  const Geometry::Vec3D BC(beamFC.getCentre());
  const Geometry::Vec3D BY(beamFC.getY());
  
  beamFC.setConnect(0,BC-BY*(length/2.0),-BY);
  beamFC.setConnect(1,BC+BY*(length/2.0),BY);

  beamFC.setLinkSurf(0,-SMap.realSurf(houseIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(houseIndex+2));
  return;
}

void
ChopperUnit::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("ChopperUnit","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(beamFC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  RS->createAll(System,FixedGroup::getKey("Main"),0);

  return;
}
  
}  // NAMESPACE constructSystem
