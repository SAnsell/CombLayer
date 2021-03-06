/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/PreDipole.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "InnerZone.h"

#include "PreDipole.h"

namespace xraySystem
{

PreDipole::PreDipole(const std::string& Key) : 
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedGroup("Tube","FlangeA","FlangeB"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  buildZone(*this,cellIndex),
  bendZone(*this,cellIndex),
  exitZone(*this,cellIndex)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(1,"Flange");
  FixedComp::nameSideIndex(2,"Photon");
  FixedComp::nameSideIndex(3,"Electron");
}


PreDipole::~PreDipole() 
  /*!
    Destructor
  */
{}

void
PreDipole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("PreDipole","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  straightLength=Control.EvalVar<double>(keyName+"StraightLength");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");
  electronAngle=Control.EvalVar<double>(keyName+"ElectronAngle");
  
  flangeARadius=Control.EvalVar<double>(keyName+"FlangeARadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeALength");
  flangeBRadius=Control.EvalVar<double>(keyName+"FlangeBRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}

void
PreDipole::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PreDipole","createSurface");

  // Do outer surfaces (vacuum ports)

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  strEnd=Origin+Y*straightLength;
  
  ModelSupport::buildPlane(SMap,buildIndex+101,strEnd,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  
  // mid layer divider
  makePlane("midDivider",SMap,buildIndex+10,Origin,X);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*radius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*radius,Z);

  // wall    
  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(radius+wallThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(radius+wallThick),Z);

  // build extention plane system
  
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(-electronAngle,Z);

  const Geometry::Vec3D XElec=QR.makeRotate(X);
  const Geometry::Vec3D YElec=QR.makeRotate(Y);
  
  const Geometry::Vec3D cylCentre=strEnd+X*(radius+electronRadius);

  // divider plane
  ModelSupport::buildPlane
    (SMap,buildIndex+110,cylCentre-X*(electronRadius/2.0),X);
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,cylCentre,Z,electronRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+117,cylCentre+X*wallThick,Z,electronRadius);

  // END plane
  const double xDisp=(1.0-cos(M_PI*electronAngle/180.0))*electronRadius;
  const double yDisp=sin(M_PI*electronAngle/180.0)*electronRadius;
  strEnd=Origin+Y*straightLength;
  cylEnd=strEnd+X*xDisp+Y*yDisp;
  elecAxis=YElec;
  
  ModelSupport::buildPlane(SMap,buildIndex+102,cylEnd,YElec);
  SurfMap::setSurf("electronCut",SMap.realSurf(buildIndex+102));
  
  // Exit cylinder [levels]
  
  ModelSupport::buildPlane(SMap,buildIndex+210,cylEnd,XElec);
  ModelSupport::buildCylinder
    (SMap,buildIndex+207,cylEnd,YElec,radius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,cylEnd,YElec,radius+wallThick);
  
  
  // flange cylinder/planes
  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,flangeARadius);
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin+Y*flangeALength,Y);
  SurfMap::setSurf("frontFlangeTube",-SMap.realSurf(buildIndex+1007));
  SurfMap::setSurf("frontFlange",SMap.realSurf(buildIndex+1001));

  ModelSupport::buildCylinder
    (SMap,buildIndex+2007,Origin,Y,flangeBRadius);
  ModelSupport::buildPlane
    (SMap,buildIndex+2001,Origin+Y*(length-flangeBLength),Y);
  SurfMap::setSurf("endFlangeTube",-SMap.realSurf(buildIndex+2007));
  SurfMap::setSurf("endFlange",SMap.realSurf(buildIndex+2001));

  return;
}

void
PreDipole::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PreDipole","createObjects");

  std::string Out;
  
  const std::string frontSurf(ExternalCut::getRuleStr("front"));

  // Construct the inner zone a a innerZone
  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 17 ");
  buildZone.setSurround(HeadRule(Out));
  buildZone.setFront(HeadRule(SMap.realSurf(buildIndex+1001)));
  buildZone.setBack(HeadRule(-SMap.realSurf(buildIndex+101)));
  buildZone.setInnerMat(0);
  buildZone.constructMasterCell(System);

    // Construct the second inner zone a
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1007 (-117: -115: 116:  (17 -10))");

  bendZone.setSurround(HeadRule(Out));
  bendZone.setFront(HeadRule(SMap.realSurf(buildIndex+101)));
  bendZone.setBack(HeadRule(-SMap.realSurf(buildIndex+102)));
  bendZone.setInnerMat(voidMat);
  bendZone.constructMasterCell(System);

  // Construct the third inner zone a
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1007 ((17 -10): (217 210): -115 : 116) ");
  exitZone.setSurround(HeadRule(Out));
  exitZone.setFront(HeadRule(SMap.realSurf(buildIndex+102)));
  exitZone.setBack(HeadRule(-SMap.realSurf(buildIndex+2001)));
  exitZone.setInnerMat(voidMat);
  exitZone.constructMasterCell(System);

  // cylinder 
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -7  ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out+frontSurf);

  // photon full
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -2 -7 -10 ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out);

  // electron  curve
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 101 -102 107 105 -106 -110 ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out);

  // electron straight
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 102 -2 (-207:-210) 105 -106  -110 ");
  makeCell("void",System,cellIndex++,voidMat,0.0,Out);

  // WALLS:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -101 7 -17 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out+frontSurf);

  // photon full
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-10 101 -2 -17 7 ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out);

  // electron  curve
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 101 -102 117 115 -116 (-107:-105:106) -110");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out);

    // electron straight
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"10 102 -2 (-217:-210) 115 -116 ((210 207):-105:106) ");
  makeCell("wall",System,cellIndex++,wallMat,0.0,Out);
    
  // front flange
  Out=ModelSupport::getComposite(SMap,buildIndex," -1001 -1007 17 ");
  makeCell("frontFlange",System,cellIndex++,flangeMat,0.0,Out+frontSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1001 1007 -2007");
  makeCell("frontFlangeVoid",System,cellIndex++,0,0.0,Out+frontSurf);
  // creat outer flange unit
  Out=ModelSupport::getComposite(SMap,buildIndex," -1001 -2007");
  setCutSurf("frontFlange",Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1001 -101 17 -2007");
  //makeCell("frontOuterVoid",System,cellIndex++,0,0.0,Out);

  // back flange
  Out=ModelSupport::getComposite 
    (SMap,buildIndex,"  2001 -2 -2007 (( 17 -10 ) : (217 210) : -115: 116) ");
  makeCell("backFlange",System,cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2001 -2007");
  setCutSurf("backFlange",Out);

  //  mid outer void
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 -2007 (( 17 -10 ) : -117:-115:116) -110");
  //  makeCell("outerVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 102 -2001 -2007 (( 17 -10 ) : (210 217) :-115:116) ");
  //  makeCell("outerVoid",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -2007 ");

  addOuterSurf("Tube",Out);
  addOuterSurf("FlangeA",Out+frontSurf);
  Out=ModelSupport::getComposite(SMap,buildIndex," -2007 -2");
  addOuterSurf("FlangeB",Out);
      

  return;
}

void 
PreDipole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("PreDipole","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);

  // centreline 
  setConnect(1,Origin+Y*length,Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+2));

  // Photon centre line [exit]
  setConnect(2,Origin+Y*length,Y);
  setLinkSurf(2,SMap.realSurf(buildIndex+2));

  // electron surface is intersect from 102 normal into surface 2
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+2));
  FixedComp::setLineConnect(3,cylEnd,elecAxis);
  
  // pipe cutters for Magnets etc:

  setConnect(4,Origin+Y*(straightLength/2.0),Z);
  setLinkSurf(4,SMap.realSurf(buildIndex+17));

  //  mid outer void
  std::string Out=ModelSupport::getComposite
    (SMap,buildIndex," (( 17 -10 ) : -117:-115:116) -110");
  setConnect(5,Origin+Y*(straightLength),Z);
  setLinkSurf(5,Out);

  // electron straight
  Out=ModelSupport::getComposite
    (SMap,buildIndex," ((17 -10):(217 210):-115:116) ");
  setConnect(6,cylEnd,elecAxis);
  setLinkSurf(6,Out);

  // Mid Half straight (half) [ divider is +10]
  HeadRule HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17");
  setConnect(7,Origin+Y*straightLength,Z);
  setLinkSurf(7,HR.complement());

  // Mid Half straight (half)  [ divider is -10 ]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"117 115 -116");
  setConnect(8,Origin+Y*straightLength,Z);
  setLinkSurf(8,HR.complement());

  // Mid Half straight (half) [ divider is +10]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"115 -116 (-217:-210)");
  setConnect(9,cylEnd,elecAxis);
  setLinkSurf(9,HR.complement());

  return;
}
  
void
PreDipole::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("PreDipole","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
