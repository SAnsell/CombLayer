/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/R3ChokeChamber.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Line.h"
#include "Plane.h"
#include "Cylinder.h"
#include "SurInter.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
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
#include "ExternalCut.h"

#include "R3ChokeChamber.h"

namespace constructSystem
{

R3ChokeChamber::R3ChokeChamber(const std::string& Key) :
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedGroup("Main","Photon","Electron","Inlet","Side"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"Photon");
  nameSideIndex(3,"Electron");
  nameSideIndex(4,"Side");
}

  
R3ChokeChamber::~R3ChokeChamber() 
  /*!
    Destructor
  */
{}

void
R3ChokeChamber::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("R3ChokeChamber","populate");
  
  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  length=Control.EvalVar<double>(keyName+"Length");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  inletWidth=Control.EvalVar<double>(keyName+"InletWidth");
  inletHeight=Control.EvalVar<double>(keyName+"InletHeight");
  inletLength=Control.EvalVar<double>(keyName+"InletLength");
  inletThick=Control.EvalVar<double>(keyName+"InletThick");
  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");

  electronXStep=Control.EvalVar<double>(keyName+"ElectronXStep");
  electronXYAngle=Control.EvalVar<double>(keyName+"ElectronXYAngle");
  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");
  electronLength=Control.EvalVar<double>(keyName+"ElectronLength");
  electronThick=Control.EvalVar<double>(keyName+"ElectronThick");
  flangeElectronRadius=Control.EvalVar<double>(keyName+"FlangeElectronRadius");
  flangeElectronLength=Control.EvalVar<double>(keyName+"FlangeElectronLength");

  photonXStep=Control.EvalVar<double>(keyName+"PhotonXStep");
  photonXYAngle=Control.EvalVar<double>(keyName+"PhotonXYAngle");
  photonRadius=Control.EvalVar<double>(keyName+"PhotonRadius");
  photonLength=Control.EvalVar<double>(keyName+"PhotonLength");
  photonThick=Control.EvalVar<double>(keyName+"PhotonThick");
  flangePhotonRadius=Control.EvalVar<double>(keyName+"FlangePhotonRadius");
  flangePhotonLength=Control.EvalVar<double>(keyName+"FlangePhotonLength");

  sideRadius=Control.EvalVar<double>(keyName+"SideRadius");
  sideLength=Control.EvalVar<double>(keyName+"SideLength");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  flangeSideRadius=Control.EvalVar<double>(keyName+"FlangeSideRadius");
  flangeSideLength=Control.EvalVar<double>(keyName+"FlangeSideLength");

 voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
 wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
 flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
 
  return;
}

void
R3ChokeChamber::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("R3ChokeChamber","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // move origin to centre of main tube
  FixedComp::applyShift(0,pipeALength,0);
  return;
}


void
R3ChokeChamber::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("R3ChokeChamber","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+101,
			       Origin-Y*(pipeALength),Y);
      setFront(SMap.realSurf(buildIndex+101));
    }

  // centre dividers
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);

  // Main cylinder
  //---------------
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  SurfMap::addSurf("VoidCyl",-SMap.realSurf(buildIndex+7));
    
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);
  SurfMap::addSurf("WallCyl",SMap.realSurf(buildIndex+17));

  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,flangeRadius);
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(length/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(length/2.0),Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(length/2.0-flangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(length/2.0-flangeLength),Z);

  // Inlet Pipe (100)
  //-----------------
  const Geometry::Quaternion photonQ=
    Geometry::Quaternion::calcQRotDeg(photonXYAngle,Z);
  const Geometry::Vec3D PX=photonQ.makeRotate(X);
  const Geometry::Vec3D PY=photonQ.makeRotate(Y);
  const Geometry::Vec3D POrigin(Origin+X*photonXStep);
  
  ModelSupport::buildCylinder(SMap,buildIndex+127,Origin,Y,flangeARadius);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(inletWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(inletWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(inletHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(inletHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+112,
			   Origin-Y*(inletLength/2.0-flangeInletLength),Y);

  // Electron Pipe (200)
  //--------------------

  const Geometry::Vec3D EOrigin(Origin+X*electronXStep);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,electronRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,Origin,Y,electronRadius+electronThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+227,Origin,Y,flangeElectronRadius);

  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(electronLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+212,Origin+Y*(electronLength/2.0-flangeElectronLength),Y);

  // photon Pipe (300)
  //---------------
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,photonRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,Y,photonRadius+photonThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+327,Origin,Y,flangePhotonRadius);

  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(photonLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+312,Origin+Y*(photonLength/2.0-flangePhotonLength),Y);

  // side Pipe (400)
  //---------------
  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,X,sideRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+417,Origin,X,sideRadius+sideThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+427,Origin,X,flangeSideRadius);

  ModelSupport::buildPlane(SMap,buildIndex+402,Origin-X*(sideLength/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+412,Origin-X*(sideLength/2.0-flangeSideLength),X);

  
  return;
}

void
R3ChokeChamber::createObjects(Simulation& System)
  /*!
    Builds all the components of the chock unit
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R3ChokeChamber","createObjects");

  const std::string frontSurf(frontRule());
    
  std::string Out;

  // Main Chamber

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -7  ");
  makeCell("MainVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 7 -17  ");
  makeCell("MainWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -15 17 -27  ");
  makeCell("MainFlangeA",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -6 16 17 -27  ");
  makeCell("MainFlangeB",System,cellIndex++,flangeMat,0.0,Out);

  //  Inlet pipe
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 103 -104 105 -106 7 -1 ");
  makeCell("InletVoid",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 113 -114 115 -116 (-103:104:-105:106) 7 -1 ");
  makeCell("InletWall",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-113:114:-115:116) -127 -112 ");
  makeCell("InletFlange",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-113:114:-115:116) -127 112 17 -1 ");
  makeCell("InletOuterVoid",System,cellIndex++,0,0.0,Out);


  //  photon pipe
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 203 -204 205 -206 7 1 -202");
  makeCell("PhotonVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 213 -214 215 -216 (-203:204:-205:206) 7 1 -202 ");
  makeCell("PhotonWall",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-213:214:-215:216) -227 212 -202 ");
  makeCell("PhotonFlange",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-213:214:-215:216) -227 -212 17 1 ");
  makeCell("PhotonOuterVoid",System,cellIndex++,0,0.0,Out);


  //  electron pipe
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 203 -204 205 -206 7 1 -202");
  makeCell("PhotonVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 213 -214 215 -216 (-203:204:-205:206) 7 1 -202 ");
  makeCell("PhotonWall",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-213:214:-215:216) -227 212 -202 ");
  makeCell("PhotonFlange",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-213:214:-215:216) -227 -212 17 1 ");
  makeCell("PhotonOuterVoid",System,cellIndex++,0,0.0,Out);



  return;
}

void
R3ChokeChamber::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("R3ChokeChamber","createLinks");

  // port centre
  FrontBackCut::createFrontLinks(*this,Origin,Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  
  // getlinke points
  FixedComp::setConnect(2,FixedComp::getLinkPt(1),-Y);
  FixedComp::setConnect(3,FixedComp::getLinkPt(2),Y);

  // make a composite flange
  std::string Out;
  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -107 ");
  FixedComp::setLinkComp(2,Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -207 ");
  FixedComp::setLinkComp(3,Out+backSurf);

  
  return;
}

  

void
R3ChokeChamber::insertAllInCell(Simulation& System,const int cellN)
  /*!
    Overload of containdGroup so that the ports can also 
    be inserted if needed
  */
{
  ContainedGroup::insertAllInCell(System,cellN);
  if (!delayPortBuild)
    {
      for(const portItem& PC : Ports)
	PC.insertInCell(System,cellN);
    }
  return;
}
  
  
void
R3ChokeChamber::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("R3ChokeChamber","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();

  insertObjects(System);
  if (!delayPortBuild)
    createPorts(System);

  return;
}
  
}  // NAMESPACE constructSystem
