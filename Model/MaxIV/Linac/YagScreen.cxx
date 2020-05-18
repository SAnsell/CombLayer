/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/YagScreen.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "BaseMap.h"
#include "CellMap.h"
#include "Exception.h"
#include "Quaternion.h"

#include "YagScreen.h"

namespace tdcSystem
{

YagScreen::YagScreen(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  screenCentreActive(false),
  pipeSide(""),pipeFront(""),
  inBeam(false)
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

YagScreen::YagScreen(const YagScreen& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedRotate(A),
  attachSystem::CellMap(A),
  jbLength(A.jbLength),jbWidth(A.jbWidth),jbHeight(A.jbHeight),
  jbWallThick(A.jbWallThick),
  jbWallMat(A.jbWallMat),
  jbMat(A.jbMat),
  ftLength(A.ftLength),
  ftInnerRadius(A.ftInnerRadius),
  ftWallThick(A.ftWallThick),
  ftFlangeLen(A.ftFlangeLen),
  ftFlangeRadius(A.ftFlangeRadius),
  ftWallMat(A.ftWallMat),
  threadLift(A.threadLift),
  threadRad(A.threadRad),
  threadMat(A.threadMat),
  mirrorRadius(A.mirrorRadius),
  mirrorAngle(A.mirrorAngle),
  mirrorThick(A.mirrorThick),
  mirrorMat(A.mirrorMat),
  screenOffset(A.screenOffset),
  screenRadius(A.screenRadius),
  screenAngle(A.screenAngle),
  screenHolderRadius(A.screenHolderRadius),
  screenHolderThick(A.screenHolderThick),
  screenHolderMat(A.screenHolderMat),
  screenCentreActive(A.screenCentreActive),
  screenCentre(A.screenCentre),
  pipeSide(A.pipeSide),pipeFront(A.pipeFront),
  voidMat(A.voidMat),inBeam(A.inBeam)
  /*!
    Copy constructor
    \param A :: YagScreen to copy
  */
{}

YagScreen&
YagScreen::operator=(const YagScreen& A)
  /*!
    Assignment operator
    \param A :: YagScreen to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      jbLength=A.jbLength;
      jbWidth=A.jbWidth;
      jbHeight=A.jbHeight;
      jbWallThick=A.jbWallThick;
      jbWallMat=A.jbWallMat;
      jbMat=A.jbMat;
      ftLength=A.ftLength;
      ftInnerRadius=A.ftInnerRadius;
      ftWallThick=A.ftWallThick;
      ftFlangeLen=A.ftFlangeLen;
      ftFlangeRadius=A.ftFlangeRadius;
      ftWallMat=A.ftWallMat;
      threadLift=A.threadLift;
      threadRad=A.threadRad;
      threadMat=A.threadMat;
      mirrorRadius=A.mirrorRadius;
      mirrorAngle=A.mirrorAngle;
      mirrorThick=A.mirrorThick;
      mirrorMat=A.mirrorMat;
      screenOffset=A.screenOffset;
      screenRadius=A.screenRadius;
      screenAngle=A.screenAngle;
      screenHolderRadius=A.screenHolderRadius;
      screenHolderThick=A.screenHolderThick;
      screenHolderMat=A.screenHolderMat;
      screenCentreActive=A.screenCentreActive;
      screenCentre=A.screenCentre;
      pipeSide=A.pipeSide;
      pipeFront=A.pipeFront;
      voidMat=A.voidMat;
      inBeam=A.inBeam;
    }
  return *this;
}

YagScreen*
YagScreen::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new YagScreen(*this);
}

YagScreen::~YagScreen()
  /*!
    Destructor
  */
{}

void
YagScreen::calcThreadLength()
  /*!
    Internal function to calculate screen thread lift based
    on the screenCentre point [if set]
  */
{
  ELog::RegMethod RegA("YagScreen","calcThreadLength");

  if (screenCentreActive)
    {
      const Geometry::Vec3D DVec=screenCentre-Origin;
      threadLift=std::abs(DVec.dotProd(Y));
      threadLift -= mirrorRadius*cos(mirrorAngle*M_PI/180.0);
    }

  return;
}


void
YagScreen::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("YagScreen","populate");

  FixedRotate::populate(Control);

  jbLength=Control.EvalVar<double>(keyName+"JBLength");
  jbWidth=Control.EvalVar<double>(keyName+"JBWidth");
  jbHeight=Control.EvalVar<double>(keyName+"JBHeight");
  jbWallThick=Control.EvalVar<double>(keyName+"JBWallThick");
  jbWallMat=ModelSupport::EvalMat<int>(Control,keyName+"JBWallMat");
  jbMat=ModelSupport::EvalMat<int>(Control,keyName+"JBMat");

  ftLength=Control.EvalVar<double>(keyName+"FTLength");
  ftInnerRadius=Control.EvalVar<double>(keyName+"FTInnerRadius");
  ftWallThick=Control.EvalVar<double>(keyName+"FTWallThick");
  ftFlangeLen=Control.EvalVar<double>(keyName+"FTFlangeLength");
  ftFlangeRadius=Control.EvalVar<double>(keyName+"FTFlangeRadius");
  ftWallMat=ModelSupport::EvalMat<int>(Control,keyName+"FTWallMat");
  threadLift=Control.EvalVar<double>(keyName+"ThreadLift");
  threadRad=Control.EvalVar<double>(keyName+"ThreadRadius");
  threadMat=ModelSupport::EvalMat<int>(Control,keyName+"ThreadMat");
  mirrorRadius=Control.EvalVar<double>(keyName+"MirrorRadius");
  mirrorAngle=Control.EvalVar<double>(keyName+"MirrorAngle");
  mirrorThick=Control.EvalVar<double>(keyName+"MirrorThick");
  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  screenOffset=Control.EvalVar<double>(keyName+"ScreenOffset");
  screenRadius=Control.EvalVar<double>(keyName+"ScreenRadius");
  screenAngle=Control.EvalVar<double>(keyName+"ScreenAngle");
  screenHolderRadius=Control.EvalVar<double>(keyName+"ScreenHolderRadius");
  screenHolderThick=Control.EvalVar<double>(keyName+"ScreenHolderThick");
  screenHolderMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenHolderMat");
  screenCentreActive=Control.EvalDefVar<int>(keyName+"ScreenCentreActive",
   					     screenCentreActive);

  if (threadRad>=ftInnerRadius)
    throw ColErr::RangeError<double>(threadRad,0,ftInnerRadius,
				     "ThreadRad >= FTInnerRadius:");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  inBeam=Control.EvalVar<int>(keyName+"InBeam");
  return;
}

void
YagScreen::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("YagScreen","createSurfaces");

  const double threadZStep(inBeam ? threadLift : 1.0);

  // linear pneumatics feedthrough
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*ftLength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,ftInnerRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,Origin,Y,ftInnerRadius+ftWallThick);

  // flange
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*ftFlangeLen,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,ftFlangeRadius);

  // electronics junction box
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+Y*(ftLength+jbWallThick),Y);
  // ModelSupport::buildPlane
  //   (SMap,buildIndex+102,Origin+Y*(ftLength+jbWallThick+jbLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(ftLength+jbWallThick+jbLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(jbWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(jbWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(jbHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(jbHeight/2.0),Z);


  SMap.addMatch(buildIndex+111, SMap.realSurf(buildIndex+2));

  ModelSupport::buildPlane
    (SMap,buildIndex+112,Origin+Y*(ftLength+jbLength+2.0*jbWallThick),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+113,Origin-X*(jbWallThick+jbWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+114,Origin+X*(jbWallThick+jbWidth/2.0),X);


  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(jbWallThick+jbHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(jbWallThick+jbHeight/2.0),Z);

  // screen+mirror thread
  ModelSupport::buildPlane
    (SMap,buildIndex+201,Origin-Y*threadZStep,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,threadRad);

  // mirror

  const Geometry::Quaternion QV =
    Geometry::Quaternion::calcQRotDeg(mirrorAngle,Z);
  const Geometry::Vec3D MVec=QV.makeRotate(X);

  const double c = cos(mirrorAngle*M_PI/180.0);

  const Geometry::Vec3D or303(Origin-X*(mirrorThick/2.0/c)-Y*(threadZStep));
  ModelSupport::buildPlane(SMap,buildIndex+303,or303,MVec);

  const Geometry::Vec3D or304(Origin+X*(mirrorThick/2.0/c)-Y*(threadZStep));
  ModelSupport::buildPlane(SMap,buildIndex+304,or304,MVec);

  // tmp is intersect of mirror cylinder and thread
  const Geometry::Vec3D tmp = (mirrorAngle < 0.0) ? or303 : or304;
  const Geometry::Vec3D or307(tmp+MVec*Z*mirrorRadius+MVec*mirrorThick/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+307,
			      or307,
			      MVec,mirrorRadius);

  // container cell for mirror and screen
  ModelSupport::buildShiftedPlane(SMap,buildIndex+301,
				  SMap.realPtr<Geometry::Plane>(buildIndex+201),
				  -mirrorRadius*2.0);

  // screen
  Geometry::Vec3D SVec(X);
  const Geometry::Quaternion SV = Geometry::Quaternion::calcQRotDeg(screenAngle,Z);
  SV.rotate(SVec);

  const Geometry::Vec3D or403(Origin+X*(screenOffset)-Y*(threadZStep));
  ModelSupport::buildPlane(SMap,buildIndex+403,or403,SVec);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+404,
				  SMap.realPtr<Geometry::Plane>(buildIndex+403),
				  screenHolderThick);

  ModelSupport::buildPlane(SMap,buildIndex+405,or403-Z*(screenHolderRadius),Z);
  ModelSupport::buildPlane(SMap,buildIndex+406,or403+Z*(screenHolderRadius),Z);

  const Geometry::Vec3D or407(or403+SVec*Z*mirrorRadius+SVec*mirrorThick/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+407,or407,SVec,screenRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+408,or407,SVec,screenHolderRadius);

  ModelSupport::buildPlane(SMap,buildIndex+401,or407,-SVec*Z);

  return;
}

void
YagScreen::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("YagScreen","createObjects");

  std::string Out;

  // linear pneumatics feedthrough
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 207 -7 ");
  makeCell("FTInner",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 7 -17 ");
  makeCell("FTWall",System,cellIndex++,ftWallMat,0.0,Out);

  // flange
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -11 17 -27 ");
  makeCell("FTFlange",System,cellIndex++,ftWallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -2 17 -27 ");
  makeCell("FTFlangeAir",System,cellIndex++,0,0.0,Out);

  // electronics junction box
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("JBVoid",System,cellIndex++,jbMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"111 -112 113 -114 115 -116 (-101:102:-103:104:-105:106)");
  makeCell("JBWall",System,cellIndex++,jbWallMat,0.0,Out);

  // Outer surfaces:
  // if junction box is larger than the feedthrough flange then
  // we can simplify outer surface by adding a rectangular cell around the flange
  if (std::min(jbWidth/2.0+jbWallThick, jbHeight/2.0+jbWallThick)>ftFlangeRadius)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1 -2 113 -114 115 -116 27");
      makeCell("JBVoidFT",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1 -112 113 -114 115 -116 ");
      addOuterSurf(Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -27 ");
      addOuterSurf(Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex," 111 -112 113 -114 115 -116 ");
      addOuterUnionSurf(Out);
    }


  // mirror/screen thread
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -207 ");
  makeCell("Thread",System,cellIndex++,threadMat,0.0,Out);

  if (inBeam)
    {
      // cells inside pipe:
      if (pipeSide.empty())
	{
	  throw ColErr::ExitAbort("pipeSide surface not set. "
				  "Use the YagScreen::setPipeSide method.");
	}
      if (pipeFront.empty())
	{
	  throw ColErr::ExitAbort("pipeFront surface not set. "
				  "Use the YagScreen::setPipeFront method.");
	}

      Out=ModelSupport::getComposite(SMap,buildIndex," 201 -1 -207 ");
      makeCell("ThreadInsidePipe",System,cellIndex++,threadMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex," 201 -1 207 ")  + pipeSide;
      makeCell("VoidThreadInsidePipe",System,cellIndex++,voidMat,0.0,Out);

      // mirror
      Out=ModelSupport::getComposite(SMap,buildIndex," 303 -304 -307 ");
      makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,Out);

      // screen
      Out=ModelSupport::getComposite(SMap,buildIndex," 403 -404 -407 ");
      makeCell("ScreenHolder",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex," 403 -404 405 -406 408 401 -201 ");
      makeCell("ScreenHolderSides",System,cellIndex++,screenHolderMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex," 403 -404 407 -408 ");
      makeCell("ScreenHolderBase",System,cellIndex++,screenHolderMat,0.0,Out);

      // screen outer surface
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 403 -404 (-408 : 405 -406 408 401 -201 ) ");
      HeadRule screenOuter;
      screenOuter.procString(Out);
      screenOuter.makeComplement();

      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 301 -201 (-303:304:307) ") + screenOuter.display();

      makeCell("ScreenAndMirrorContainer",System,cellIndex++,0,0.0,Out+pipeSide);

      Out=ModelSupport::getComposite(SMap,buildIndex," -301 ")  + pipeFront + pipeSide;
      makeCell("VoidFrontInsidePipe",System,cellIndex++,voidMat,0.0,Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -1 ")  + pipeFront + pipeSide;
      makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
    }

  return;
}


void
YagScreen::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("YagScreen","createLinks");




  return;
}

void
YagScreen::setScreenCentre(const attachSystem::FixedComp& FC,
			   const long int sIndex)
  /*!
    Set the screen centre
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
{
  ELog::RegMethod RegA("YagScreen","setScreenCentre");

  screenCentreActive=1;
  screenCentre=FC.getLinkPt(sIndex);

  return;
}

void
YagScreen::setPipeSide(const attachSystem::FixedComp& FC,
			 const long int sIndex)
/*
  Set the side surface of the pipe containing mirror and screen holder
  \param FC :: FixedComp to use
  \param sIndex :: Link point index
 */
{
  ELog::RegMethod RegA("YagScreen","setPipeSide");

  pipeSide = FC.getLinkString(sIndex);
}

void
YagScreen::setPipeFront(const attachSystem::FixedComp& FC,
			 const long int sIndex)
/*
  Set the front surface of the pipe containing mirror and screen holder
  \param FC :: FixedComp to use
  \param sIndex :: Link point index
 */
{
  ELog::RegMethod RegA("YagScreen","setPipeFront");

  pipeFront = FC.getLinkString(sIndex);
}

void
YagScreen::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("YagScreen","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  calcThreadLength();
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
