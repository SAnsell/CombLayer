/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3Common/EPContinue.cxx
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "SimProcess.h"
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
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "PointMap.h" 

#include "EPCombine.h"
#include "EPContinue.h"

namespace xraySystem
{

EPContinue::EPContinue(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::PointMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key Name
  */
{
  nameSideIndex(1,"Flange");
  nameSideIndex(2,"Photon");
  nameSideIndex(3,"Electron");
}


EPContinue::~EPContinue() 
  /*!
    Destructor
  */
{}

void
EPContinue::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EPContinue","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  lengthB=Control.EvalVar<double>(keyName+"LengthB");

  electronRadius=Control.EvalVar<double>(keyName+"ElectronRadius");

  photonAGap=Control.EvalVar<double>(keyName+"PhotonAGap")/2.0;
  photonBGap=Control.EvalVar<double>(keyName+"PhotonBGap")/2.0;
  photonStep=Control.EvalVar<double>(keyName+"PhotonStep");

  photonWidth=Control.EvalVar<double>(keyName+"PhotonWidth");
  height=Control.EvalVar<double>(keyName+"Height");

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
EPContinue::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EPContinue","createSurface");

  // calc XAxis since depends on Z:
  elecXAxis=elecYAxis*Z;

  // Do outer surfaces (vacuum ports)
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!hasPoint("PhotonSide"))
    setPoint("PhotonSide",Origin-X*photonWidth);
  
  if (!hasPoint("ElectronSide"))
    setPoint("ElectronSide",Origin+X*photonWidth);
  
  const Geometry::Vec3D eSidePt=getPoint("ElectronSide");
  const Geometry::Vec3D pSidePt=getPoint("PhotonSide");
  
  ModelSupport::buildPlane(SMap,buildIndex+3,pSidePt-X*photonWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,eSidePt,elecXAxis);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  
  // Main outer cylinder
  SurfMap::makeCylinder("voidCyl",SMap,buildIndex+7,Origin,Y,outerRadius);

  // electron
  SurfMap::makeCylinder
    ("elecRadius",SMap,buildIndex+107,elecOrg,elecYAxis,electronRadius);

  // PHOTON
  // photon is a cone expanding from photonAGap -> photonBGap (effective radii)
  // back step  (photonAxis == Y):

  // mid divider
  ModelSupport::buildPlane(SMap,buildIndex+200,photonOrg,X);

  // outer cone
  const double backStep=(length*photonAGap)/(photonBGap-photonAGap);
  const Geometry::Vec3D axisPoint(photonOrg-Y*backStep);
  const double angle=std::atan(photonBGap/(backStep+length));
  SurfMap::makeCone("photonCone",SMap,buildIndex+208,
		    axisPoint,Y,angle*180.0/M_PI);
  // inner cone
  // const double backStepB=(length*photonAGap)/(photonBGap+photonStep-photonAGap);
  // const Geometry::Vec3D axisPointB(photonOrg-Y*backStepB);
  // const double angleB=std::atan((photonBGap+photonStep)/(backStepB+length));
  SurfMap::makeCone("photonConeB",SMap,buildIndex+209,
		    axisPoint+X*photonStep,Y,angle*180.0/M_PI);


  // SECOND FRAME
  // Mid line
  
  ModelSupport::buildPlane(SMap,buildIndex+1002,Origin+Y*(length+lengthB),Y);
  // Main photonCylinder
  SurfMap::makeCylinder("photonCyl",SMap,buildIndex+1208,Origin,Y,photonBGap);
  // Closing photonCylinder
  const Geometry::Vec3D PAxis(Y*lengthB-X*photonStep);
  SurfMap::makeCylinder("photonConeB",SMap,buildIndex+1209,
			photonOrg+Y*length+X*photonStep,PAxis,photonBGap);
  
  
  return;
}

void
EPContinue::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EPContinue","createObjects");

  HeadRule HR;
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6 107 208 (-200:209)");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2 -1002 3 -4 5 -6 107 1208 1209");
  makeCell("WallB",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -1002 -107");
  makeCell("electronVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 (-208:(200 -209))");
  makeCell("photonVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "2 -1002 (-1208 : -1209)");
  makeCell("photonVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -1002 3 -4 5 -6");
  addOuterSurf(HR);

  
  return;
}

void 
EPContinue::createLinks()
  /*!
    Create the linked units
  */
{
  ELog::RegMethod RegA("EPContinue","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  /*
  // photon/electron
  setConnect(1,Origin+Y*length,Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+2));

  setConnect(2,photonOrg-X*photonXStep+Y*length,Y);  
  setLinkSurf(2,SMap.realSurf(buildIndex+2));
  
  // electron surface is intersect from 102 normal into surface 2
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+2));
  FixedComp::setLineConnect(3,elecOrg,elecYAxis);
  */
  return;
}

void
EPContinue::setEPOriginPair(const EPCombine& EP)
  /*!
    Set the electron/Photon origins exactly
    \param EP :: EPCombined channel system is joined to

   */
{
  ELog::RegMethod RegA("EPContinue","setEPOriginPair(string)");

  setEPOriginPair(EP,"Photon","Electron");

  setPoint("PhotonSide",EP.getLinkPt("PhotonEdge"));
  setPoint("ElectronSide",EP.getLinkPt("ElectronEdge"));
  
  return;
}

void
EPContinue::setEPOriginPair(const attachSystem::FixedComp& FC,
			    const std::string& photonIndex,
			    const std::string& electronIndex)
  /*!
    Set the electron/Photon origins exactly
    \param FC :: FixedPoint
    \param photonIndex :: link point for photon
    \param electornIndex :: link point for electron
   */
{
  ELog::RegMethod RegA("EPContinue","setEPOriginPair(string)");

  setEPOriginPair(FC,FC.getSideIndex(photonIndex),
		  FC.getSideIndex(electronIndex));

  return;
}

void
EPContinue::setEPOriginPair(const attachSystem::FixedComp& FC,
			    const long int photonIndex,
			    const long int electronIndex)
  /*!
    Set the electron/Photon origins exactly
    \param FC :: FixedPoint
    \param photonIndex :: link point for photon
    \param electornIndex :: link point for electron
   */
{
  ELog::RegMethod RegA("EPContinue","setEPOriginPair");

  photonOrg=FC.getLinkPt(photonIndex);
  elecOrg=FC.getLinkPt(electronIndex);

  elecYAxis=FC.getLinkAxis(electronIndex);
  epPairSet=1;
  
  return;
}

void
EPContinue::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("EPContinue","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
