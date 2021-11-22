/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3Common/HalfElectronPipe.cxx
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

#include "Exception.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "HalfElectronPipe.h"

namespace xraySystem
{

HalfElectronPipe::HalfElectronPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("Half","Full"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(1,"Flange");
  FixedComp::nameSideIndex(2,"Photon");
  FixedComp::nameSideIndex(3,"Electron");
}


HalfElectronPipe::~HalfElectronPipe()
  /*!
    Destructor
  */
{}

void
HalfElectronPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("HalfElectronPipe","populate");

  FixedRotate::populate(Control);


  fullLength=Control.EvalVar<double>(keyName+"FullLength");
  photonRadius=Control.EvalVar<double>(keyName+"PhotonRadius");
  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  electronAngle=Control.EvalVar<double>(keyName+"ElectronAngle");
  
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}


void
HalfElectronPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("HalfElectronPipe","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*fullLength,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // mid layer divider
  makePlane("midDivider",SMap,buildIndex+10,Origin,X);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,photonRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin,Y,photonRadius+wallThick);
  
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*photonRadius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*photonRadius,Z);

  // wall    (top / base)
  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(photonRadius+wallThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(photonRadius+wallThick),Z);

  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(-electronAngle,Z);

  const Geometry::Vec3D XElec=QR.makeRotate(X);
  const Geometry::Vec3D YElec=QR.makeRotate(Y);
  
  const Geometry::Vec3D cylCentre=Origin+X*(photonRadius+electronRadius);

    // divider plane
  makePlane("electronDivide",SMap,buildIndex+110,
	    cylCentre-X*(electronRadius/2.0),X);

  // Note cyinder goes wrong way relative to inner void:
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,cylCentre,Z,electronRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+117,cylCentre+X*wallThick,Z,electronRadius);

    // END plane
  const double xDisp=(1.0-cos(M_PI*electronAngle/180.0))*electronRadius;
  const double yDisp=sin(M_PI*electronAngle/180.0)*electronRadius;
  endPoint=Origin+X*xDisp+Y*yDisp;
  elecAxis=YElec;  
  makePlane("electronCut",SMap,buildIndex+102,endPoint,elecAxis);
  ExternalCut::setCutSurf("mid",-SMap.realSurf(buildIndex+102));
      
  // for full pipe:
  ModelSupport::buildPlane(SMap,buildIndex+210,endPoint,XElec);
  ModelSupport::buildCylinder
    (SMap,buildIndex+207,endPoint,YElec,photonRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+217,endPoint,YElec,photonRadius+wallThick);
  
  return;
}

void
HalfElectronPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("HalfElectronPipe","createObjects");

  const HeadRule& frontHR=getRule("front");
  const HeadRule& midHR=getRule("mid");
  const HeadRule& backHR=getRule("back");

  HeadRule HR;

  // Inner void (x2):
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-10 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*midHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"10 105 -106 107 -110");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*midHR);

  // Wall (x2)
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-10 -17 7");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*frontHR*midHR);
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"10 -110 115 -116 117 (-105 : 106 : -107)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*frontHR*midHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-17 -10) : (10 115 -116 117 -110)");
  ExternalCut::setCutSurf("HalfElectron",HR);
  addOuterSurf("Half",HR*frontHR*midHR);

  const HeadRule midComp=midHR.complement();
  // Full pipe
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-10 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*midComp*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"10 -210 105 -106");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*midComp*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"210 -207 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*midComp*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-10 7 -17");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*midComp*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"10 -210 115 -116 (-105:106)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*midComp*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"210 -217 207");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR*midComp*backHR);
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-10 -17) : (210 -217) : (10 -210 115 -116)");
  ExternalCut::setCutSurf("FullElectron",HR);
  addOuterSurf("Full",HR*midComp*backHR);
  return;
}


void
HalfElectronPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("HalfElectronPipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  // Photon centre line [exit]
  setConnect(2,Origin+Y*fullLength,Y);
  setLinkSurf(2,SMap.realSurf(buildIndex+2));

  setLinkSurf(3,SMap.realSurf(buildIndex+2));
  setLineConnect(3,endPoint,elecAxis);

  FixedComp::nameSideIndex(4,"midPlane");
  setConnect(4,endPoint,elecAxis);
  setLinkSurf(4,SMap.realSurf(buildIndex+102));


  return;
}

void
HalfElectronPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("HalfElectronPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
