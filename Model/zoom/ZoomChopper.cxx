/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomChopper.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "SurInter.h"
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
#include "shutterBlock.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h" 
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "GeneralShutter.h"
#include "BulkShield.h"
#include "bendSection.h"
#include "ZoomBend.h"
#include "ZoomChopper.h"

namespace zoomSystem
{

ZoomChopper::ZoomChopper(const std::string& Key) : 
  attachSystem::FixedGroup(Key,"Main",6,"Beam",2),
  attachSystem::ContainedComp(),
  nShield(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomChopper::ZoomChopper(const ZoomChopper& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedComp(A),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  length(A.length),depth(A.depth),height(A.height),
  leftWidth(A.leftWidth),rightWidth(A.rightWidth),
  leftAngle(A.leftAngle),rightAngle(A.rightAngle),
  voidLeftWidth(A.voidLeftWidth),
  voidRightWidth(A.voidRightWidth),voidHeight(A.voidHeight),
  voidDepth(A.voidDepth),voidCut(A.voidCut),
  voidChanLeft(A.voidChanLeft),voidChanRight(A.voidChanRight),
  voidChanUp(A.voidChanUp),voidChanDown(A.voidChanDown),
  nShield(A.nShield),wallMat(A.wallMat),
  roofBStep(A.roofBStep),roofHeight(A.roofHeight),
  roofExtra(A.roofExtra),roofMat(A.roofMat),
  roofExtraMat(A.roofExtraMat),monoWallSurf(A.monoWallSurf),
  voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: ZoomChopper to copy
  */
{}

ZoomChopper&
ZoomChopper::operator=(const ZoomChopper& A)
  /*!
    Assignment operator
    \param A :: ZoomChopper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      length=A.length;
      depth=A.depth;
      height=A.height;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
      leftAngle=A.leftAngle;
      rightAngle=A.rightAngle;
      voidLeftWidth=A.voidLeftWidth;
      voidRightWidth=A.voidRightWidth;
      voidHeight=A.voidHeight;
      voidDepth=A.voidDepth;
      voidCut=A.voidCut;
      voidChanLeft=A.voidChanLeft;
      voidChanRight=A.voidChanRight;
      voidChanUp=A.voidChanUp;
      voidChanDown=A.voidChanDown;
      nShield=A.nShield;
      wallMat=A.wallMat;
      roofBStep=A.roofBStep;
      roofHeight=A.roofHeight;
      roofExtra=A.roofExtra;
      roofMat=A.roofMat;
      roofExtraMat=A.roofExtraMat;
      monoWallSurf=A.monoWallSurf;
      voidCell=A.voidCell;
    }
  return *this;
}

ZoomChopper::~ZoomChopper() 
  /*!
    Destructor
  */
{}


void
ZoomChopper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("ZoomChopper","populate");

  // extract value if needed for isolated case
  outerRadius=Control.EvalDefVar<double>("bulkOuterRadius",0.0);
  outerOffset=Control.EvalDefVar<double>(keyName+"OuterOffset",0.0);
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  length=Control.EvalVar<double>(keyName+"Length");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");

  leftWidth=Control.EvalVar<double>(keyName+"LeftWidth");
  rightWidth=Control.EvalVar<double>(keyName+"RightWidth");
  leftAngle=Control.EvalVar<double>(keyName+"LeftAngle");
  rightAngle=Control.EvalVar<double>(keyName+"RightAngle");
  leftWaxAngle=Control.EvalDefVar<double>(keyName+"LeftWaxAngle",leftAngle);
  rightWaxAngle=Control.EvalDefVar<double>(keyName+"RightWaxAngle",rightAngle);

  waxLeftSkin=Control.EvalVar<double>(keyName+"WaxLeftSkin");
  waxRightSkin=Control.EvalVar<double>(keyName+"WaxRightSkin");

  voidLeftWidth=Control.EvalVar<double>(keyName+"VoidLeftWidth");
  voidRightWidth=Control.EvalVar<double>(keyName+"VoidRightWidth");
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidCut=Control.EvalVar<double>(keyName+"VoidCut");

  voidChanLeft=Control.EvalVar<double>(keyName+"VoidChanLeft");
  voidChanRight=Control.EvalVar<double>(keyName+"VoidChanRight");
  voidChanUp=Control.EvalVar<double>(keyName+"VoidChanUp");
  voidChanDown=Control.EvalVar<double>(keyName+"VoidChanDown");

  voidEnd=Control.EvalVar<double>(keyName+"VoidEnd");  
  voidEndLeft=Control.EvalVar<double>(keyName+"VoidEndLeft");
  voidEndRight=Control.EvalVar<double>(keyName+"VoidEndRight");
  voidEndUp=Control.EvalVar<double>(keyName+"VoidEndUp");
  voidEndDown=Control.EvalVar<double>(keyName+"VoidEndDown");
  voidEndMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidEndMat");

  if (voidEndMat!=0)
    {
      voidEndBeamLeft=Control.EvalVar<double>(keyName+"VoidEndBeamLeft");
      voidEndBeamRight=Control.EvalVar<double>(keyName+"VoidEndBeamRight");
      voidEndBeamUp=Control.EvalVar<double>(keyName+"VoidEndBeamUp");
      voidEndBeamDown=Control.EvalVar<double>(keyName+"VoidEndBeamDown");
    }

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  waxMat=ModelSupport::EvalMat<int>(Control,keyName+"WaxMat");

  nShield=Control.EvalVar<size_t>(keyName+"NShield");

  // SHIELD IS BASIC:
  
  ModelSupport::populateDivide(Control,nShield,keyName+"ShieldMat_",
   			       voidEndMat,shieldMat);
  ModelSupport::populateDivideLen(Control,nShield,
				  keyName+"ShieldLen_",
				  voidEnd,shieldFrac);

  return;
}

void
ZoomChopper::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("ZoomChopper","createLinks");

  setDefault("Main","Beam");
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.setConnect(1,Origin+bY*length,bY);
  mainFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));

  beamFC.setConnect(0,Origin,-bY);
  beamFC.setConnect(1,Origin+bY*length,bY);
  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));

  Geometry::Vec3D LNorm(X);
  Geometry::Quaternion::calcQRotDeg(-leftAngle,Z).rotate(LNorm);  
  Geometry::Vec3D RNorm(X);
  Geometry::Quaternion::calcQRotDeg(rightAngle,Z).rotate(RNorm);  
    
  mainFC.setConnect(2,Origin+Y*length-X*leftWidth,LNorm);     
  mainFC.setConnect(3,Origin+Y*length+X*rightWidth,RNorm);
  mainFC.setLinkSurf(2,SMap.realSurf(buildIndex+3));
  mainFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));

  // Link point is the join between BeamOrigin+
  // beam exit
  //setBeamExit(buildIndex+2,bEnter,bY);
  return;
}

void
ZoomChopper::createUnitVector(const zoomSystem::ZoomBend& ZB)
  /*!
    Create the unit vectors
    \param ZB :: Zoom Bender
  */
{
  ELog::RegMethod RegA("ZoomChopper","createUnitVector");
  
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(ZB.getKey("Main"));
  beamFC.createUnitVector(ZB.getKey("Beam"));

  mainFC.setCentre(beamFC.getCentre());
  mainFC.applyShift(xStep,yStep,zStep);
  beamFC.applyShift(xStep,yStep,zStep);

  return;
}

void
ZoomChopper::createSurfaces(const shutterSystem::GeneralShutter& GS)
  /*!
    Create All the surfaces
    \param GS :: GeneralShutter [for divide]
  */
{
  ELog::RegMethod RegA("ZoomChopper","createSurfaces(GS)");
  
  
  SMap.addMatch(buildIndex+100,GS.getDivideSurf());
  SMap.addMatch(buildIndex+1,monoWallSurf);
  createSurfacesCommon();
  return;
}

void
ZoomChopper::createSurfaces()
  /*!
    Create an new effective mono-wall
  */
{
  ELog::RegMethod RegA("ZoomChopper","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin+Y*outerOffset,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+1,Origin+Y*outerOffset,
			      Z,outerRadius);     
  createSurfacesCommon();
  return;
}
  
void
ZoomChopper::createSurfacesCommon()
  /*!
    Create the common surfaces
  */
{
  ELog::RegMethod RegA("ZoomChopper","createSurfacesCommon");

  setDefault("Main","Beam");
  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+
			   Y*length,Y);     // back plane
  
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+3,
				  Origin+Y*length-X*leftWidth,X,
				  Z,-leftAngle);     
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+4,
				  Origin+Y*length+X*rightWidth,X,
				  Z,rightAngle);     

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+113,
				  Origin+Y*length-
				  X*(leftWidth+waxLeftSkin),X,
				  Z,-leftWaxAngle);     
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+114,
				  Origin+Y*length+
				  X*(rightWidth+waxRightSkin),X,
				  Z,rightWaxAngle);     

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z); 
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  mainFC.setConnect(4,Origin-Z*depth,Z);     
  mainFC.setConnect(5,Origin+Z*height,Z);
  mainFC.setLinkSurf(4,SMap.realSurf(buildIndex+5));
  mainFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));

  // Void ::
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*voidCut,Y);     

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*voidChanLeft,X);     
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*voidChanRight,X);       
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*voidChanDown,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*voidChanUp,Z);


  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin+Y*(length-voidEnd),Y);     
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*voidLeftWidth,X);     
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*voidRightWidth,X);     
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*voidHeight,Z);

  
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*voidEndLeft,X);     
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*voidEndRight,X);       
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*voidEndDown,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*voidEndUp,Z);

  
  // Cut through

  
  if (voidEndMat>0)
    {
      Geometry::Vec3D VECent=bOrigin+bY*(length-voidEnd);
      ModelSupport::buildPlane(SMap,buildIndex+1033,
			       VECent-bX*voidEndBeamLeft,bX);     
      ModelSupport::buildPlane(SMap,buildIndex+1034,
			       VECent+bX*voidEndBeamRight,bX);       
      ModelSupport::buildPlane(SMap,buildIndex+1035,VECent-bZ*voidEndBeamDown,bZ);
      ModelSupport::buildPlane(SMap,buildIndex+1036,VECent+bZ*voidEndBeamUp,bZ);
    }
  return;
}
  
  
void
ZoomChopper::createObjects(Simulation& System,
			   const attachSystem::ContainedGroup& CObj)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
    \param CObj :: Contained object
   */
{
  ELog::RegMethod RegA("ZoomChopper","createObjects");

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex,"100 1 -2 113 -114 5 -6");
  addOuterSurf(Out);      

  //Front steel
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "100 1 -11 3 -4 5 -6 (-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  //Mid Step
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -21 3 -4 5 -6 (-23:24:-25:26) ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  // Far Step
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "21 -2 3 -4 5 -6 (-33:34:-35:36) ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  // voids:
  Out=ModelSupport::getComposite(SMap,buildIndex,"100 1 -11 13 -14 15 -16");
  Out+=CObj.getExclude("B");
  Out+=CObj.getExclude("C");
  Out+=CObj.getExclude("D");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -21 23 -24 25 -26");
  Out+=CObj.getExclude("D");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  voidCell=cellIndex-1;


  if (!voidEndMat)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"21 -2 33 -34 35 -36");
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
      shieldCell=cellIndex-1;
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"21 -2 33 -34 35 -36"
				     "(-1033 : 1034 : -1035 :1036) ");
      System.addCell(MonteCarlo::Object(cellIndex++,voidEndMat,0.0,Out));
      shieldCell=cellIndex-1;
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "21 -2 1033 -1034 1035 -1036 ");
      System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    }

  // WAX
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "100 1 -2 113 -3 5 -6  ");
  System.addCell(MonteCarlo::Object(cellIndex++,waxMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "100 1 -2 -114 4 5 -6  ");
  System.addCell(MonteCarlo::Object(cellIndex++,waxMat,0.0,Out));

  return;
}

  
void
ZoomChopper::divideMainShield(Simulation& System)
  /*!
    Construct the vane object
    \param System :: System object
   */
{
  ELog::RegMethod RegA("ZoomChopper","divideMainShield");

  if (nShield>1)
    {
      ModelSupport::surfDivide DA;
      for(size_t i=1;i<nShield;i++)
	{
	  DA.addFrac(shieldFrac[i-1]);
	  DA.addMaterial(shieldMat[i-1]);
	}
      DA.addMaterial(shieldMat.back());

      DA.init();
      DA.setCellN(shieldCell);
      DA.setOutNum(cellIndex,buildIndex+3001);

      // Modern divider system:
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> wallRule;
      wallRule.setSurfPair(SMap.realSurf(buildIndex+21),
			   SMap.realSurf(buildIndex+2));
      
      const std::string OutA=
	ModelSupport::getComposite(SMap,buildIndex," 21 ");
      const std::string OutB=
	ModelSupport::getComposite(SMap,buildIndex," -2 ");
      wallRule.setInnerRule(OutA);
      wallRule.setOuterRule(OutB);
  
      DA.addRule(&wallRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

void
ZoomChopper::createAll(Simulation& System,
		       const zoomSystem::ZoomBend& ZB)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZB :: Zoom Bender
  */
{
  ELog::RegMethod RegA("ZoomChopper","createAll(Iso)");
  
  populate(System.getDataBase());
  createUnitVector(ZB);
  createSurfaces();
  createObjects(System,ZB);
  divideMainShield(System);
    
  createLinks();
  insertObjects(System);   
  
  return;
}

void
ZoomChopper::createAll(Simulation& System,
		       const zoomSystem::ZoomBend& ZB,
		       const shutterSystem::GeneralShutter& GShutter)	       
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZB :: Zoom Bender
    \param GShutter :: General Shutter [for direction]
  */
{
  ELog::RegMethod RegA("ZoomChopper","createAll");
  
  populate(System.getDataBase());
  createUnitVector(ZB);
  createSurfaces(GShutter);
  createObjects(System,ZB);
  divideMainShield(System);
  createLinks();
  insertObjects(System);
  
  
  return;
}
  
}  // NAMESPACE shutterSystem
