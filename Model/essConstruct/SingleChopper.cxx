/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/SingleChopper.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "SingleChopper.h"

namespace constructSystem
{

SingleChopper::SingleChopper(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Main",6,"Beam",2,"BuildBeam",0),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),
  houseIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(houseIndex+1),
  motor(new constructSystem::Motor(Key+"Motor")),
  frontFlange(new constructSystem::boltRing(Key,"FrontFlange")),
  backFlange(new constructSystem::boltRing(Key,"BackFlange")),
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

  OR.addObject(motor);
  OR.addObject(frontFlange);
  OR.addObject(backFlange);
  OR.addObject(RS);
  OR.addObject(IPA);
  OR.addObject(IPB);
}

SingleChopper::SingleChopper(const SingleChopper& A) : 
  attachSystem::FixedOffsetGroup(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  houseIndex(A.houseIndex),cellIndex(A.cellIndex),
  height(A.height),
  width(A.width),depth(A.depth),length(A.length),
  shortHeight(A.shortHeight),shortWidth(A.shortWidth),
  mainRadius(A.mainRadius),mainThick(A.mainThick),
  boltMat(A.boltMat),wallMat(A.wallMat),
  RS(new RingSeal(*A.RS)),IPA(new InnerPort(*A.IPA)),
  IPB(new InnerPort(*A.IPB))
  /*!
    Copy constructor
    \param A :: SingleChopper to copy
  */
{}

SingleChopper&
SingleChopper::operator=(const SingleChopper& A)
  /*!
    Assignment operator
    \param A :: SingleChopper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      height=A.height;
      width=A.width;
      depth=A.depth;
      length=A.length;
      shortHeight=A.shortHeight;
      shortWidth=A.shortWidth;
      mainRadius=A.mainRadius;
      mainThick=A.mainThick;
      boltMat=A.boltMat;
      wallMat=A.wallMat;
      *RS=*A.RS;
      *IPA=*A.IPA;
      *IPB=*A.IPB;
    }
  return *this;
}

SingleChopper::~SingleChopper() 
  /*!
    Destructor
  */
{}

void
SingleChopper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("SingleChopper","populate");

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

  
  
  boltMat=ModelSupport::EvalMat<int>(Control,keyName+"BoltMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
SingleChopper::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("SingleChopper","createUnitVector");

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
SingleChopper::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("SingleChopper","createSurfaces");

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
  
  return;
}
  
void
SingleChopper::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SingleChopper","createObjects");

  const attachSystem::FixedComp& BuildBeam=getKey("BuildBeam");
  
  std::string Out,FBStr,EdgeStr,SealStr;

    // Main void
  Out=ModelSupport::getComposite(SMap,houseIndex,"11 -12 -17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("Void",cellIndex-1);

  // Main block
  Out=ModelSupport::getComposite(SMap,houseIndex,
                "1 -2 3 -4 5 -6 7 8 9 10 (-11:12:17)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Wall",cellIndex-1);
  addCell("MainBlock",cellIndex-1);


  // Front ring seal
  frontFlange->setInnerExclude();
  frontFlange->addInsertCell(getCell("MainBlock"));
  frontFlange->setFront(SMap.realSurf(houseIndex+1));
  frontFlange->setBack(-SMap.realSurf(houseIndex+11));
  frontFlange->createAll(System,BuildBeam,0);

  backFlange->setInnerExclude();
  backFlange->addInsertCell(getCell("MainBlock"));
  backFlange->setFront(SMap.realSurf(houseIndex+12));
  backFlange->setBack(-SMap.realSurf(houseIndex+2));
  backFlange->createAll(System,BuildBeam,0);

  // Port [Front/back]
  const std::string innerFSurf=
    std::to_string(-frontFlange->getSurf("innerRing"));
  const std::string innerBSurf=
    std::to_string(-backFlange->getSurf("innerRing"));
  Out=ModelSupport::getComposite(SMap,houseIndex," 1 -11 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+innerFSurf));
  addCell("PortVoid",cellIndex-1);

  IPA->addInnerCell(getCell("PortVoid",0));
  IPA->createAll(System,BuildBeam,0,Out+innerFSurf);

  
  Out=ModelSupport::getComposite(SMap,houseIndex,"12 -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+innerBSurf));
  addCell("PortVoid",cellIndex-1);

  IPB->addInnerCell(getCell("PortVoid",1));
  IPB->createAll(System,BuildBeam,0,Out+innerBSurf);

  
  // Outer
  Out=ModelSupport::getComposite(SMap,houseIndex,"1 -2 3 -4 5 -6 7 8 9 10");
  addOuterSurf(Out);  
  
  return;
}

void
SingleChopper::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("SingleChopper","createLinks");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  setDefault("Main");

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
SingleChopper::insertAxle(Simulation& System,
			const attachSystem::CellMap& CM) const
  /*!
    Accessor function to allow the axle to be put in the 
    disks
    \param System :: Simulation to use
    \param CM :: Cell Map to get disks from -- requires
    named cells (Inner).
  */
{
  ELog::RegMethod RegA("SingleChopper","insertAxle");

  motor->insertInCell("Axle",System,CM.getCells("Inner"));

  return;
}

void
SingleChopper::createMotor(Simulation& System)
  /*!
    Create the motor
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("SingleChopper","procMotor");

  RS->createAll(System,FixedGroup::getKey("Main"),0);
  
  
  motor->addInsertCell("Plate",getCell("MainBlock"));
  motor->addInsertCell("Axle",getCell("Void"));

  motor->setInnerPlanes(SMap.realSurf(houseIndex+11),
			-SMap.realSurf(houseIndex+12));
  motor->setYSteps(mainThick/2.0,mainThick/2.0);
  motor->createAll(System,FixedGroup::getKey("Main"),0);

  addSurf("MotorAxle",motor->getSurf("axle"));
  return;
}


void
SingleChopper::createAll(Simulation& System,
                       const attachSystem::FixedComp& beamFC,
                       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param FIndex :: side index
  */
{
  ELog::RegMethod RegA("SingleChopper","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(beamFC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();

  // LOTS of care here because insertObjects removes insert cells
  motor->addInsertCell("Outer",*this);
  insertObjects(System);

  createMotor(System);
  return;
}
  
}  // NAMESPACE constructSystem
