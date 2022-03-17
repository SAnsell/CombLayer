/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/BiPortTube.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#include <iomanip>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <set>
#include <map>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Line.h"
#include "Cylinder.h"
#include "SurInter.h"
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
#include "SurfMap.h"
#include "ExternalCut.h"
#include "SurInter.h"

#include "portItem.h"
#include "doublePortItem.h"
#include "BiPortTube.h"

namespace xraySystem
{

BiPortTube::BiPortTube(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedGroup("Left","Right"),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
    
  nameSideIndex(2,"FrontA");
  nameSideIndex(3,"FrontB");
  
  nameSideIndex(4,"OutA");
  nameSideIndex(5,"OutB");  
}
  

BiPortTube::~BiPortTube()
  /*!
    Destructor
  */
{}

void
BiPortTube::setLeftPort(const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Simple copy for left inlet port
    \param FC :: Fixed Comp to get left port from
    \param sideIndex :: Link point set set
   */
{
  // Side MUST be defined:
  setLinkCopy(2,FC,sideIndex);
  return;
}

void
BiPortTube::setRightPort(const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Simple copy for right inlet port 
    \param FC :: Fixed Comp to get left port from
    \param sideIndex :: Link point set 
  */
{
  setLinkCopy(3,FC,sideIndex);
  return;
}

void
BiPortTube::setLeftPort(const attachSystem::FixedComp& FC,
			const std::string& sideName)
  /*!
    Simple copy for left inlet port 
    \param FC :: Fixed Comp to get left port from
    \param sideName :: Link point set 
   */
{
  setLinkCopy(2,FC,sideName);
  return;
}

void
BiPortTube::setRightPort(const attachSystem::FixedComp& FC,
			 const std::string& sideName)
  /*!
    Simple copy for left inlet port 
    \param FC :: Fixed Comp to get left port from
    \param sideName :: Link point set 
   */
{
  setLinkCopy(3,FC,sideName);
  return;
}


void
BiPortTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("BiPortTube","populate");


  FixedRotate::populate(Control);
  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  length=Control.EvalVar<double>(keyName+"Length");
  
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  capThick=Control.EvalVar<double>(keyName+"CapThick");

  outLength=Control.EvalVar<double>(keyName+"OutLength");

  inPortRadius=Control.EvalVar<double>(keyName+"InPortRadius");
  outPortRadius=Control.EvalVar<double>(keyName+"OutPortRadius");
  inWallThick=Control.EvalVar<double>(keyName+"InWallThick");
  outWallThick=Control.EvalVar<double>(keyName+"OutWallThick");
  inFlangeRadius=Control.EvalVar<double>(keyName+"InFlangeRadius");
  outFlangeRadius=Control.EvalVar<double>(keyName+"OutFlangeRadius");
  inFlangeLength=Control.EvalVar<double>(keyName+"InFlangeLength");
  outFlangeLength=Control.EvalVar<double>(keyName+"OutFlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  capMat=ModelSupport::EvalMat<int>(Control,keyName+"CapMat");

  return;
}

void
BiPortTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("BiPortTube","createSurfaces");

  // Side MUST be defined:
  if (!hasLinkSurf("FrontA") || !hasLinkSurf("FrontB"))
    throw ColErr::EmptyContainer("Left/Right Link");
    
  setCutSurf("frontA",*this,"FrontA");
  setCutSurf("frontB",*this,"FrontB");

  SurfMap::makePlane("MidDivider",SMap,buildIndex+100,Origin,Y);
  // MAIN Cylinder::
  
  SurfMap::makeCylinder("#VoidCyl",SMap,buildIndex+7,Origin,Z,radius);
  SurfMap::makeCylinder
    ("OuterCyl",SMap,buildIndex+17,Origin,Z,radius+wallThick);
  SurfMap::makeCylinder("Flange",SMap,buildIndex+27,Origin,Z,flangeRadius);


  SurfMap::makePlane("Base",SMap,buildIndex+5,Origin-Z*(length/2.0),Z);
  SurfMap::makePlane("Top",SMap,buildIndex+6,Origin+Z*(length/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+105,
			   Origin-Z*(length/2.0-flangeLength),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
			   Origin+Z*(length/2.0-flangeLength),Z);

  if (capThick>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,buildIndex+205,
			       Origin-Z*(length/2.0+capThick),Z);
      ModelSupport::buildPlane(SMap,buildIndex+206,
			       Origin+Z*(length/2.0+capThick),Z);
    }

  
  // FRONT PIPES:
  const Geometry::Vec3D beamAPt(getLinkPt("FrontA"));
  const Geometry::Vec3D beamBPt(getLinkPt("FrontB"));
  const Geometry::Vec3D beamAAxis(getLinkAxis("FrontA"));
  const Geometry::Vec3D beamBAxis(getLinkAxis("FrontB"));
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+1007,beamAPt,beamAAxis,inPortRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+2007,beamBPt,beamBAxis,inPortRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+1017,beamAPt,beamAAxis,inPortRadius+inWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+2017,beamBPt,beamBAxis,inPortRadius+inWallThick);

  ModelSupport::buildCylinder
    (SMap,buildIndex+1027,beamAPt,beamAAxis,inFlangeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+2027,beamBPt,beamBAxis,inFlangeRadius);

  ExternalCut::makeShiftedSurf(SMap,"frontA",buildIndex+1011,
			    beamAAxis,inFlangeLength);
  ExternalCut::makeShiftedSurf(SMap,"frontB",buildIndex+2011,
			       beamBAxis,inFlangeLength);


  // BACK PIPES:

  ModelSupport::buildCylinder
    (SMap,buildIndex+3007,beamAPt,beamAAxis,outPortRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+4007,beamBPt,beamBAxis,outPortRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+3017,beamAPt,beamAAxis,outPortRadius+outWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+4017,beamBPt,beamBAxis,outPortRadius+outWallThick);
  ModelSupport::buildCylinder
    (SMap,buildIndex+3027,beamAPt,beamAAxis,outFlangeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+4027,beamBPt,beamBAxis,outFlangeRadius);

  // calculate exit point:

  const Geometry::Vec3D outPtA=
    SurInter::getLinePoint(beamAPt,beamAAxis,buildIndex+17,Origin+Y*radius);
  const Geometry::Vec3D outPtB=
    SurInter::getLinePoint(beamBPt,beamBAxis,buildIndex+17,Origin+Y*radius);

  SurfMap::makePlane("OutA",SMap,buildIndex+3002,
		     outPtA+beamAAxis*outLength,beamAAxis);
  SurfMap::makePlane("OutB",SMap,buildIndex+4002,
		     outPtB+beamBAxis*outLength,beamBAxis);

  SurfMap::makePlane("OutA",SMap,buildIndex+3012,
		     outPtA+beamAAxis*(outLength-outFlangeLength),beamAAxis);
  SurfMap::makePlane("OutB",SMap,buildIndex+4012,
		     outPtB+beamBAxis*(outLength-outFlangeLength),beamBAxis);

  
  return;
}

void
BiPortTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BiPortTube","createObjects");

  const HeadRule aHR=ExternalCut::getRule("frontA");
  const HeadRule bHR=ExternalCut::getRule("frontB");

  HeadRule HR;
  // MAIN cylinder:
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"5 -6 7 -17 (100:(1007 2007)) (-100:(3007 4007))");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"105 -106 17 -27 (100:(1017 2017)) (-100:(3017 4017))");
  makeCell("OuterWall",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-105 5 -27 17");
  makeCell("BaseFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"106 -6 -27 17");
  makeCell("TopFlange",System,cellIndex++,wallMat,0.0,HR);

  // Front Ports

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 7 -1007");
  makeCell("AVoid",System,cellIndex++,voidMat,0.0,HR*aHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 7 -2007");
  makeCell("BVoid",System,cellIndex++,voidMat,0.0,HR*bHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 17 -1017 1007");
  makeCell("AWall",System,cellIndex++,wallMat,0.0,HR*aHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 17 -2017 2007");
  makeCell("BWall",System,cellIndex++,wallMat,0.0,HR*bHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1027 1017 -1011");
  makeCell("AFlange",System,cellIndex++,wallMat,0.0,HR*aHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2027 2017 -2011");
  makeCell("BFlange",System,cellIndex++,wallMat,0.0,HR*bHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 27 -1027 1017 1011");
  makeCell("AOuter",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 27 -2027 2017 2011");
  makeCell("BOuter",System,cellIndex++,0,0.0,HR);

  
  //   Back Ports
  // --------------

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 7 -3007 -3002");
  makeCell("CVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 7 -4007 -4002");
  makeCell("DVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 17 -3017 3007 -3002");
  makeCell("CWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 17 -4017 4007 -4002");
  makeCell("DWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3027 3017 3012 -3002");
  makeCell("CFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-4027 4017 4012 -4002");
  makeCell("DFlange",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 27 -3027 3017 -3012");
  makeCell("COuter",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 27 -4027 4017 -4012");
  makeCell("DOuter",System,cellIndex++,0,0.0,HR);


  // EXTERIOR

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 -1027");
  addOuterSurf("Left",HR*aHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 -2027");
  addOuterSurf("Right",HR*bHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 -3027 -3002");
  addOuterUnionSurf("Left",HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 -4027 -4002");
  addOuterUnionSurf("Right",HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-27  5 -6");
  addOuterUnionSurf("Left",HR);
  addOuterUnionSurf("Right",HR);

  return;
}

void
BiPortTube::createLinks()
  /*!
    Create the linkes
  */
{
  ELog::RegMethod RegA("BiPortTube","createLinks");

  // front back set to mid point:
  const Geometry::Vec3D beamAPt(getLinkPt("FrontA"));
  const Geometry::Vec3D beamBPt(getLinkPt("FrontB"));
  const Geometry::Vec3D beamAAxis(getLinkAxis("FrontA"));  
  const Geometry::Vec3D beamBAxis(getLinkAxis("FrontB"));

  FixedComp::setConnect(0,(beamAPt+beamBPt)/2.0,(beamAAxis+beamBAxis)/2.0);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+17));
  
  
  const Geometry::Vec3D beamCPt=
    SurInter::getLinePoint(beamAPt,beamAAxis,buildIndex+3002,Origin+Y*radius);
  const Geometry::Vec3D beamDPt=
    SurInter::getLinePoint(beamBPt,beamBAxis,buildIndex+4002,Origin+Y*radius);

  FixedComp::setConnect(1,(beamCPt+beamDPt)/2.0,-(beamAAxis+beamBAxis)/2.0);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+17));  

  // set link points to front/back begin/exit ports

  ExternalCut::createLink("frontA",*this,2,beamAPt,-beamAAxis);
  ExternalCut::createLink("frontB",*this,3,beamBPt,-beamBAxis);

  FixedComp::setConnect(4,beamCPt,beamAAxis);
  FixedComp::setNamedLinkSurf(4,"outA",SMap.realSurf(buildIndex+3002));  


  FixedComp::setConnect(5,beamDPt,beamBAxis);
  FixedComp::setNamedLinkSurf(5,"outB",SMap.realSurf(buildIndex+4002));  
  

  
  //  FixedComp::setConnect(1,(beamAPt+beamBPt)/2.0,-(beamAAxis+beamBAxis)/2.0);
  
  

  return;
}


void
BiPortTube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("BiPortTube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);

  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE System
