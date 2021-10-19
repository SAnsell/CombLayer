/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/CooledScreen.cxx
 *
 * Copyright (c) 2004-2021 Stuart Ansell
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
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Line.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"

#include "CooledScreen.h"

namespace xraySystem
{

CooledScreen::CooledScreen(const std::string& Key)  :
  attachSystem::ContainedGroup("Payload","Connect","Outer"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  inBeam(false)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


CooledScreen::~CooledScreen()
  /*!
    Destructor
  */
{}

void
CooledScreen::calcImpactVector()
  /*!
    Calculate the impact points of the main beam  on the screen surface:
    We have the beamAxis this must intersect the screen and mirror closest to 
    their centre points. It DOES NOT need to hit the centre points as the mirror
    system is confined to moving down the Y axis of the object. 
    [-ve Y from flange  to beam centre]
  */
{
  ELog::RegMethod RegA("CooledScreen","calcImpactVector");

  // defined points:

  // This point is the beam centre point between the main axis:

  std::tie(std::ignore,screenCentre)=
    beamAxis.closestPoints(Geometry::Line(Origin,Y));

  // Thread point
  //  threadEnd=screenCentre+Y*(holderLongLen-LHalf);
  threadEnd=screenCentre+Y*(copperHeight/2.0);
  return;
}


void
CooledScreen::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CooledScreen","populate");

  FixedRotate::populate(Control);

  juncBoxLength=Control.EvalVar<double>(keyName+"JuncBoxLength");
  juncBoxWidth=Control.EvalVar<double>(keyName+"JuncBoxWidth");
  juncBoxHeight=Control.EvalVar<double>(keyName+"JuncBoxHeight");
  juncBoxWallThick=Control.EvalVar<double>(keyName+"JuncBoxWallThick");
  feedLength=Control.EvalVar<double>(keyName+"FeedLength");
  
  feedInnerRadius=Control.EvalVar<double>(keyName+"FeedInnerRadius");
  feedWallThick=Control.EvalVar<double>(keyName+"FeedWallThick");
  feedFlangeLen=Control.EvalVar<double>(keyName+"FeedFlangeLen");
  feedFlangeRadius=Control.EvalVar<double>(keyName+"FeedFlangeRadius");
  
  threadLift=Control.EvalVar<double>(keyName+"ThreadLift");
  threadRadius=Control.EvalVar<double>(keyName+"ThreadRadius");

  screenAngle=Control.EvalVar<double>(keyName+"ScreenAngle");
  copperWidth=Control.EvalVar<double>(keyName+"CopperWidth");
  copperHeight=Control.EvalVar<double>(keyName+"CopperHeight");
  copperThick=Control.EvalVar<double>(keyName+"CopperThick");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  screenThick=Control.EvalVar<double>(keyName+"ScreenThick");
  

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  juncBoxMat=ModelSupport::EvalMat<int>(Control,keyName+"JuncBoxMat");
  juncBoxWallMat=ModelSupport::EvalMat<int>(Control,keyName+"JuncBoxWallMat");
  threadMat=ModelSupport::EvalMat<int>(Control,keyName+"ThreadMat");

  copperMat=ModelSupport::EvalMat<int>(Control,keyName+"CopperMat");
  screenMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenMat");

  feedWallMat=ModelSupport::EvalMat<int>(Control,keyName+"FeedWallMat");

  inBeam=Control.EvalVar<int>(keyName+"InBeam");
  return;
}

void
CooledScreen::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CooledScreen","createSurfaces");

  calcImpactVector();

  // linear pneumatics feedthrough
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*feedLength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,feedInnerRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin,Y,feedInnerRadius+feedWallThick);

  // flange
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*feedFlangeLen,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,feedFlangeRadius);

  // electronics junction box
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(feedLength+juncBoxWallThick),Y);
  // ModelSupport::buildPlane
  //   (SMap,buildIndex+102,Origin+Y*(feedLength+juncBoxWallThick+juncBoxLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(feedLength+juncBoxWallThick+juncBoxLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(juncBoxWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(juncBoxWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(juncBoxHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(juncBoxHeight/2.0),Z);


  SMap.addMatch(buildIndex+111, SMap.realSurf(buildIndex+2));

  ModelSupport::buildPlane
    (SMap,buildIndex+112,Origin+Y*(feedLength+juncBoxLength+2.0*juncBoxWallThick),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+113,Origin-X*(juncBoxWallThick+juncBoxWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+114,Origin+X*(juncBoxWallThick+juncBoxWidth/2.0),X);


  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(juncBoxWallThick+juncBoxHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(juncBoxWallThick+juncBoxHeight/2.0),Z);

  // screen+mirror thread
  ModelSupport::buildPlane
    (SMap,buildIndex+201,threadEnd,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,threadRadius);

  // copper screen

  const Geometry::Quaternion QW =
      Geometry::Quaternion::calcQRotDeg(-screenAngle,Y);
  
  // holder cut plane normal
  // NOTE : Y is in Z direction
  const Geometry::Vec3D SX=QW.makeRotate(X);
  const Geometry::Vec3D SY=QW.makeRotate(Z);
  const Geometry::Vec3D SZ=Y;

  
  ModelSupport::buildPlane
    (SMap,buildIndex+2001,screenCentre-SY*(copperThick/2.0),SY);
  ModelSupport::buildPlane
    (SMap,buildIndex+2002,screenCentre+SY*(copperThick/2.0),SY);
  ModelSupport::buildPlane
    (SMap,buildIndex+2003,screenCentre-SX*(copperWidth/2.0),SX);
  ModelSupport::buildPlane
    (SMap,buildIndex+2004,screenCentre+SX*(copperWidth/2.0),SX);
  ModelSupport::buildPlane
    (SMap,buildIndex+2005,screenCentre-SZ*(copperHeight/2.0),SZ);
  ModelSupport::buildPlane
    (SMap,buildIndex+2006,screenCentre+SZ*(copperHeight/2.0),SZ);

  return;
}

void
CooledScreen::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CooledScreen","createObjects");

  HeadRule HR;

  // linear pneumatics feedthrough
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 207 -7");
  makeCell("FTInner",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 7 -17");
  makeCell("FTWall",System,cellIndex++,feedWallMat,0.0,HR);

  // flange
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -11 17 -27");
  makeCell("FTFlange",System,cellIndex++,feedWallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -2 17 -27");
  makeCell("FTFlangeAir",System,cellIndex++,0,0.0,HR);

  // electronics junction box
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 103 -104 105 -106");
  makeCell("JBVoid",System,cellIndex++,juncBoxMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"111 -112 113 -114 115 -116 (-101:102:-103:104:-105:106)");
  makeCell("JBWall",System,cellIndex++,juncBoxWallMat,0.0,HR);

  // Outer surfaces:
  // if junction box is larger than the feedthrough flange then
  // we can simplify outer surface by adding a rectangular cell around the flange
  if (std::min(juncBoxWidth/2.0+juncBoxWallThick,
	       juncBoxHeight/2.0+juncBoxWallThick) > feedFlangeRadius)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"1 -2 113 -114 115 -116 27");
      makeCell("JBVoidFT",System,cellIndex++,0,0.0,HR);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"1 -112 113 -114 115 -116");
      addOuterSurf("Outer",HR);
    }
  else
    {
      // THIS NEEDS TO BE CHANGED to HAVE the void cylinder from the flange
      // encompass the junction box
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -27");
      addOuterSurf("Outer",HR);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"111 -112 113 -114 115 -116");
      addOuterUnionSurf("Outer",HR);
    }


  // mirror/screen thread
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -207");
  makeCell("Thread",System,cellIndex++,threadMat,0.0,HR);
  if (inBeam)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -1 -207");
      makeCell("ThreadInsidePipe",System,cellIndex++,threadMat,0.0,HR);
      addOuterSurf("Connect",HR);

      // yag screen

      // HR=ModelSupport::getHeadRule(SMap,buildIndex,"2001 -2002 -2007");
      // makeCell("CooledScreen",System,cellIndex++,screenMat,0.0,HR);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"2001 -2002 2003 -2004 2005 -2006");
	makeCell("YagHolder",System,cellIndex++,copperMat,0.0,HR);

      addOuterSurf("Payload",HR);      
      
    }
      
  return;
}


void
CooledScreen::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("CooledScreen","createLinks");

  return;
}

void
CooledScreen::setBeamAxis(const attachSystem::FixedComp& FC,
			    const long int sIndex)
  /*!
    Set the screen centre
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
{
  ELog::RegMethod RegA("CooledScreen","setBeamAxis(FC)");

  beamAxis=Geometry::Line(FC.getLinkPt(sIndex),
			  FC.getLinkAxis(sIndex));


  return;
}

void
CooledScreen::setBeamAxis(const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& Axis)
  /*!
    Set the screen centre
    \param Org :: Origin point for line
    \param Axis :: Axis of line
  */
{
  ELog::RegMethod RegA("CooledScreen","setBeamAxis(Vec3D)");

  beamAxis=Geometry::Line(Org,Axis);
  return;
}

void
CooledScreen::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CooledScreen","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
