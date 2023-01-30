/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/WindowPipe.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "GeneralPipe.h"
#include "WindowPipe.h"

namespace constructSystem
{

WindowPipe::WindowPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,11),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

WindowPipe::WindowPipe(const WindowPipe& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),  
  radius(A.radius),length(A.length),feThick(A.feThick),
  claddingThick(A.claddingThick),flangeARadius(A.flangeARadius),
  flangeALength(A.flangeALength),flangeBRadius(A.flangeBRadius),
  flangeBLength(A.flangeBLength),windowA(A.windowA),
  windowB(A.windowB),voidMat(A.voidMat),feMat(A.feMat),
  claddingMat(A.claddingMat),flangeMat(A.flangeMat),
  outerVoid(A.outerVoid)
  /*!
    Copy constructor
    \param A :: WindowPipe to copy
  */
{}

WindowPipe&
WindowPipe::operator=(const WindowPipe& A)
  /*!
    Assignment operator
    \param A :: WindowPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      radius=A.radius;
      length=A.length;
      feThick=A.feThick;
      claddingThick=A.claddingThick;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      windowA=A.windowA;
      windowB=A.windowB;
      voidMat=A.voidMat;
      feMat=A.feMat;
      claddingMat=A.claddingMat;
      flangeMat=A.flangeMat;
      outerVoid=A.outerVoid;
    }
  return *this;
}

WindowPipe::~WindowPipe()
  /*!
    Destructor
  */
{}

void
WindowPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("WindowPipe","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  feThick=Control.EvalVar<double>(keyName+"FeThick");

  if (length<Geometry::zeroTol || radius<Geometry::zeroTol ||
      feThick<Geometry::zeroTol)
    {
      throw ColErr::SizeError<double>
	(radius,length,"Pipe:["+keyName+"] radius/length/feThick");
    }
  claddingThick=Control.EvalDefVar<double>(keyName+"CladdingThick",0.0);

  flangeARadius=Control.EvalHead<double>(keyName+"Flange","ARadius","Radius");
  flangeBRadius=Control.EvalHead<double>(keyName+"Flange","BRadius","Radius");

  flangeALength=Control.EvalHead<double>(keyName+"Flange","ALength","Length");
  flangeBLength=Control.EvalHead<double>(keyName+"Flange","BLength","Length");

  if (flangeALength<Geometry::zeroTol ||
      flangeBLength<Geometry::zeroTol ||
      flangeARadius<radius+claddingThick+feThick ||
      flangeBRadius<radius+claddingThick+feThick)
    {
      throw ColErr::SizeError<double>
	(flangeARadius,flangeBRadius,"Pipe:["+keyName+"] Flange");
    }

  windowA.thick=Control.EvalDefPair<double>
    (keyName+"WindowAThick",keyName+"WindowThick",0.0);
  windowA.radius=Control.EvalDefPair<double>
    (keyName+"WindowARadius",keyName+"WindowRadius",-1.0);
  windowA.height=Control.EvalDefPair<double>
    (keyName+"WindowAHeight",keyName+"WindowHeight",-1.0);
  windowA.width=Control.EvalDefPair<double>
    (keyName+"WindowAWidth",keyName+"WindowWidth",-1.0);
  windowA.mat=ModelSupport::EvalDefMat
    (Control,keyName+"WindowAMat",keyName+"WindowMat",0);

  windowB.thick=Control.EvalDefPair<double>
    (keyName+"WindowBThick",keyName+"WindowThick",0.0);
  windowB.radius=Control.EvalDefPair<double>
    (keyName+"WindowBRadius",keyName+"WindowRadius",-1.0);
  windowB.height=Control.EvalDefPair<double>
    (keyName+"WindowBHeight",keyName+"WindowHeight",-1.0);
  windowB.width=Control.EvalDefPair<double>
    (keyName+"WindowBWidth",keyName+"WindowWidth",-1.0);
  windowB.mat=ModelSupport::EvalDefMat
    (Control,keyName+"WindowBMat",keyName+"WindowMat",0);

  activeWindow=0;
  if (windowA.thick>Geometry::zeroTol)
    activeWindow ^= 1;
  if (windowB.thick>Geometry::zeroTol)
    activeWindow ^= 2;

  if (activeWindow & 1)
    {
      if (windowA.radius<Geometry::zeroTol &&
	  (windowA.width<Geometry::zeroTol ||
	   windowA.height<Geometry::zeroTol))
	throw ColErr::EmptyContainer("Pipe:["+keyName+"] has neither "
				     "windowA:Radius or Height/Width");

      if (windowA.radius>Geometry::zeroTol &&
	  windowA.radius+Geometry::zeroTol>flangeARadius)
	throw ColErr::SizeError<double>
	  (windowA.radius,flangeARadius,"Pipe:["+keyName+"] windowA.Radius/flangeARadius");
    }
  if (activeWindow & 2)
    {
      if (windowB.radius<Geometry::zeroTol &&
	  (windowB.width<Geometry::zeroTol || windowB.height<Geometry::zeroTol))
	throw ColErr::EmptyContainer("Pipe:["+keyName+"] has neither "
				     "windowB:Radius or Height/Width");

      if (windowB.radius>Geometry::zeroTol &&
	  windowB.radius+Geometry::zeroTol>flangeBRadius)
	throw ColErr::SizeError<double>
	  (windowB.radius,flangeBRadius,
	   "Pipe:["+keyName+"] windowB.Radius/flangeBRadius");
    }


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  claddingMat=ModelSupport::EvalDefMat(Control,keyName+"CladdingMat",0);
  flangeMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeMat",feMat);

  outerVoid = Control.EvalDefVar<int>(keyName+"OuterVoid",0);

  return;
}

void
WindowPipe::applyActiveFrontBack()
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("WindowPipe","applyActiveFrontBack");

  const Geometry::Vec3D curFP= 
    (frontPointActive()) ? getFrontPoint() : Origin;
  const Geometry::Vec3D curBP= (backPointActive()) ?
    getBackPoint() : Origin+Y*length;
  
  Origin=(curFP+curBP)/2.0;
  const Geometry::Vec3D YAxis=(curBP-curFP).unit();
  Geometry::Vec3D RotAxis=(YAxis*Y).unit();   // need unit for numerical acc.
  if (!RotAxis.nullVector())
    {
      const Geometry::Quaternion QR=
	Geometry::Quaternion::calcQVRot(Y,YAxis,RotAxis);
      Y=YAxis;
      QR.rotate(X);
      QR.rotate(Z);
    }
  else if (Y.dotProd(YAxis) < -0.5) // (reversed
    {
      Y=YAxis;
      X*=-1.0;
      Z*=-1.0;
    }

  return;
}

void
WindowPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("WindowPipe","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  

  // Front Inner void
  getShiftedFront(SMap,buildIndex+101,Y,flangeALength);
  if (activeWindow & 1)
    {
      getShiftedFront(SMap,buildIndex+1001,Y,
		      (flangeALength-windowA.thick)/2.0);
      getShiftedFront(SMap,buildIndex+1002,Y,
		      (flangeALength+windowA.thick)/2.0);
      // add data to surface
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1001));
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1002));
    }

  // Back Inner void
  FrontBackCut::getShiftedBack(SMap,buildIndex+102,Y,-flangeBLength);
  if (activeWindow & 2)
    {
      getShiftedBack(SMap,buildIndex+1101,Y,
		     -(flangeBLength-windowB.thick)/2.0);
      getShiftedBack(SMap,buildIndex+1102,Y,
		     -(flangeBLength+windowB.thick)/2.0);
      // add data to surface
      addSurf("BackWindow",buildIndex+1101);
      addSurf("BackWindow",buildIndex+1102);
    }

  // MAIN SURFACES:
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,
			      Y,radius+feThick+claddingThick);
  addSurf("OuterRadius",SMap.realSurf(buildIndex+27));

  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  // FRONT WINDOW SURFACES:
  if (activeWindow & 1)
    {
      if (windowA.radius>0.0)
	ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,
                                    windowA.radius);
      else
	{
	  ModelSupport::buildPlane(SMap,buildIndex+1003,
                                   Origin-X*(windowA.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1004,
                                   Origin+X*(windowA.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1005,
                                   Origin-Z*(windowA.height/2.0),Z);
	  ModelSupport::buildPlane(SMap,buildIndex+1006,
                                   Origin+Z*(windowA.height/2.0),Z);
	}
    }

  // FRONT WINDOW SURFACES:
  if (activeWindow & 2)
    {
      if (windowB.radius>Geometry::zeroTol)
	ModelSupport::buildCylinder(SMap,buildIndex+1107,Origin,Y,
                                    windowB.radius);
      else
	{
	  ModelSupport::buildPlane(SMap,buildIndex+1103,
                                   Origin-X*(windowB.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1104,
                                   Origin+X*(windowB.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+1105,
                                   Origin-Z*(windowB.height/2.0),Z);
	  ModelSupport::buildPlane(SMap,buildIndex+1106,
                                   Origin+Z*(windowB.height/2.0),Z);
	}
    }

  return;
}

void
WindowPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("WindowPipe","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  HeadRule windowAExclude;
  HeadRule windowBExclude;
  if (activeWindow & 1)      // FRONT
    {
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-1007 1003 -1004 1005 -1006 1001 -1002");
      makeCell("Window",System,cellIndex++,windowA.mat,0.0,
				       HR*getDivider("front"));
      windowAExclude=HR.complement();
    }
  if (activeWindow & 2)
    {
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-1107 1103 -1104 1105 -1106 1102 -1101");
	    
      makeCell("Window",System,cellIndex++,windowB.mat,0.0,
	       HR*getDivider("back"));
      windowBExclude=HR.complement();
    }

  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7");
  makeCell("Void",System,cellIndex++,voidMat,0.0,
	   HR*frontHR*backHR*windowAExclude*windowBExclude);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-17 13 -14 15 -16");
  const HeadRule WallLayer(HR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex," -27 23 -24 25 -26");
  const HeadRule CladdingLayer(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 7");
  makeCell("Steel",System,cellIndex++,feMat,0.0,HR*WallLayer);
  addCell("MainSteel",cellIndex-1);   
  // cladding
  if (claddingThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 ");
      HR*=WallLayer.complement()*CladdingLayer;
      makeCell("Cladding",System,cellIndex++,claddingMat,0.0,HR);
    }

  // FLANGE A: 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107 7");
  makeCell("Steel",System,cellIndex++,flangeMat,0.0,
	   HR*frontHR*windowAExclude);

  // FLANGE: 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207 7");
  makeCell("Steel",System,cellIndex++,flangeMat,0.0,
	   HR*backHR*windowBExclude);

  if (outerVoid)
    {
      if (std::abs(flangeARadius-flangeBRadius)<Geometry::zeroTol)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 27 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107");
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
      else if (flangeARadius>flangeBRadius)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 27 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 207 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*backHR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-107");
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
      else if (flangeBRadius>flangeARadius)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 27 -207 ");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 107 -207");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*frontHR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207");
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
    }
  else
    {
      // outer boundary [flange front]
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
      addOuterSurf("FlangeA",HR*frontHR);

      // outer boundary [flange back]
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207");
      addOuterSurf("FlangeB",HR*backHR);

      // outer boundary mid tube
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102");
      addOuterSurf("Main",HR*CladdingLayer);
    }

  return;
}


void
WindowPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("WindowPipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  // Round pipe
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));
  
  FixedComp::setConnect(7,Origin-Z*(radius+feThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(radius+feThick),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));
  
  FixedComp::nameSideIndex(7,"outerPipe");
  FixedComp::nameSideIndex(7,"pipeOuterBase");
  FixedComp::nameSideIndex(8,"pipeOuterTop");
  
  // MID Point: [NO SURF]
  const Geometry::Vec3D midPt=
    (getLinkPt(1)+getLinkPt(2))/2.0;
  FixedComp::setConnect(6,midPt,Y);

  FixedComp::setConnect(9,Origin-Z*flangeARadius,-Z);
  FixedComp::setConnect(10,Origin+Z*flangeARadius,Z);
  
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+107));
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+107));

  return;
}

void
WindowPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("WindowPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  applyActiveFrontBack();
  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
