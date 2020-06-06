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
#include "Exception.h"
#include "Quaternion.h"

#include "YagScreen.h"

namespace tdcSystem
{

YagScreen::YagScreen(const std::string& Key)  :
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


YagScreen::~YagScreen()
  /*!
    Destructor
  */
{}

void
YagScreen::calcImpactVector()
  /*!
    Calculate the impact points of the main beam 
    on the mirror surfaces:
    We have the beamAxis this must intersect the screen and mirror closest to 
    their centre points. It DOES NOT need to hit the centre points as the mirror
    system is confined to moving down the Y axis of the object. [-ve Y from flange 
    to beam centre]
  */
{
  ELog::RegMethod RegA("YagScreen","calcThreadLength");

  // defined points:

  // This point is the beam centre point between the main axis:

  std::tie(std::ignore,mirrorCentre)=
    beamAxis.closestPoints(Geometry::Line(Origin,Y));

  // Beam Centre point projected along -X hits the mirror at half way
  // between the short and long length [holderShortLen/holderLongLen]
  // mirror start above mirrorCentre (+Y)
  
  const double LHalf=(holderLongLen-holderShortLen)/2.0; 
  mirrorStart=mirrorCentre+Y*LHalf-X*(holderDepth/2.0);

  // projection from mirrorStart along axis 
  mirrorImpact=mirrorStart-X*(LHalf*tan(mirrorAngle*M_PI/180.0))-Y*LHalf;  

  // screen from screen start [SDist = full distance to centre]
  const double SDist=screenVOffset+(holderLongLen-holderShortLen)/2.0; 
  const Geometry::Vec3D screenStart=mirrorStart+Y*screenVOffset;

  // angle points outwards:
  screenImpact=screenStart-X*(SDist*tan(screenAngle*M_PI/180.0))-Y*SDist;  

  // Thread point
  threadEnd=mirrorCentre+Y*(holderLongLen-LHalf);

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
  holderWidth=Control.EvalVar<double>(keyName+"HolderWidth");
  holderDepth=Control.EvalVar<double>(keyName+"HolderDepth");
  holderShortLen=Control.EvalVar<double>(keyName+"HolderShortLen");
  holderLongLen=Control.EvalVar<double>(keyName+"HolderLongLen");
  
  mirrorAngle=Control.EvalVar<double>(keyName+"MirrorAngle");
  mirrorRadius=Control.EvalVar<double>(keyName+"MirrorRadius");
  mirrorThick=Control.EvalVar<double>(keyName+"MirrorThick");
  
  screenAngle=Control.EvalVar<double>(keyName+"ScreenAngle");
  screenVOffset=Control.EvalVar<double>(keyName+"ScreenVOffset");
  screenRadius=Control.EvalVar<double>(keyName+"ScreenRadius");
  screenThick=Control.EvalVar<double>(keyName+"ScreenThick");
  
  screenHolderRadius=Control.EvalVar<double>(keyName+"ScreenHolderRadius");
  screenHolderThick=Control.EvalVar<double>(keyName+"ScreenHolderThick");

  
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  juncBoxMat=ModelSupport::EvalMat<int>(Control,keyName+"JuncBoxMat");
  juncBoxWallMat=ModelSupport::EvalMat<int>(Control,keyName+"JuncBoxWallMat");
  threadMat=ModelSupport::EvalMat<int>(Control,keyName+"ThreadMat");
  holderMat=ModelSupport::EvalMat<int>(Control,keyName+"HolderMat");
  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  screenMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenMat");
  screenHolderMat=ModelSupport::EvalMat<int>(Control,keyName+"ScreenHolderMat");
  feedWallMat=ModelSupport::EvalMat<int>(Control,keyName+"FeedWallMat");

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

  calcImpactVector();
  const double threadStep(inBeam ? 0.0 : 1.0);

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

  // Screen holder:
  const Geometry::Quaternion QV =
    Geometry::Quaternion::calcQRotDeg(-mirrorAngle,Z);

  // holder cut plane normal
  const Geometry::Vec3D MX=QV.makeRotate(X);
  
  ModelSupport::buildPlane(SMap,buildIndex+1003,threadEnd-X*(holderDepth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,threadEnd+X*(holderDepth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1005,threadEnd-Z*(holderWidth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,threadEnd+Z*(holderWidth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1002,threadEnd-Y*holderLongLen,Y);

  // cut plane at mirrorAngle and on short distance
  const Geometry::Vec3D MStart(threadEnd-Y*holderShortLen-X*(holderWidth/2.0));
  const Geometry::Vec3D MCentre(threadEnd-Y*holderShortLen-X*(holderWidth/2.0));

  ModelSupport::buildPlane(SMap,buildIndex+1009,mirrorStart,MX);
  ModelSupport::buildCylinder(SMap,buildIndex+1007,mirrorImpact,MX,
			      mirrorRadius);

  ModelSupport::buildPlane(SMap,buildIndex+1019,mirrorStart+MX*mirrorThick,MX);

  // yag screen

  const Geometry::Quaternion QW =
      Geometry::Quaternion::calcQRotDeg(-screenAngle,Z);

  // holder cut plane normal
  const Geometry::Vec3D SX=QW.makeRotate(X);
  const Geometry::Vec3D SY=SX*Z;

  // dividep plane
  ModelSupport::buildPlane(SMap,buildIndex+2000,screenImpact,SY);
  ModelSupport::buildPlane(SMap,buildIndex+2001,screenImpact-SX*(screenThick/2.0),SX);
  ModelSupport::buildPlane(SMap,buildIndex+2002,screenImpact+SX*(screenThick/2.0),SX);

  ModelSupport::buildCylinder(SMap,buildIndex+2007,screenImpact,SX,screenRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+2017,screenImpact,SX,screenHolderRadius);

  ModelSupport::buildPlane(SMap,buildIndex+2013,screenImpact-Z*screenHolderRadius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2014,screenImpact+Z*screenHolderRadius,Z);

  ELog::EM<<"Mirror centre == "<<mirrorCentre<<ELog::endDiag;
  ELog::EM<<"Mirror start ==  "<<mirrorStart<<ELog::endDiag;
  ELog::EM<<"Mirror impact == "<<mirrorImpact<<ELog::endDiag;
  ELog::EM<<"Thread End ==    "<<threadEnd<<ELog::endDiag;
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
  makeCell("FTWall",System,cellIndex++,feedWallMat,0.0,Out);

  // flange
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -11 17 -27 ");
  makeCell("FTFlange",System,cellIndex++,feedWallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -2 17 -27 ");
  makeCell("FTFlangeAir",System,cellIndex++,0,0.0,Out);

  // electronics junction box
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -102 103 -104 105 -106 ");
  makeCell("JBVoid",System,cellIndex++,juncBoxMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"111 -112 113 -114 115 -116 (-101:102:-103:104:-105:106)");
  makeCell("JBWall",System,cellIndex++,juncBoxWallMat,0.0,Out);

  // Outer surfaces:
  // if junction box is larger than the feedthrough flange then
  // we can simplify outer surface by adding a rectangular cell around the flange
  if (std::min(juncBoxWidth/2.0+juncBoxWallThick,
	       juncBoxHeight/2.0+juncBoxWallThick) > feedFlangeRadius)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1 -2 113 -114 115 -116 27");
      makeCell("JBVoidFT",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1 -112 113 -114 115 -116 ");
      addOuterSurf("Outer",Out);
    }
  else
    {
      // THIS NEEDS TO BE CHANGED to HAVE the void cylinder from the flange
      // encompass the junction box
      Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -27 ");
      addOuterSurf("Outer",Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex," 111 -112 113 -114 115 -116 ");
      addOuterUnionSurf("Outer",Out);
    }


  // mirror/screen thread
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -207 ");
  makeCell("Thread",System,cellIndex++,threadMat,0.0,Out);
  if (inBeam)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 201 -1 -207 ");
      makeCell("ThreadInsidePipe",System,cellIndex++,threadMat,0.0,Out);
      addOuterSurf("Connect",Out);

      // build Mirror holder:
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1002 -201 1003 -1004 1005 -1006 1009 1007 ");
      makeCell("Holder",System,cellIndex++,holderMat,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1009 -1007 -1019");
      makeCell("Mirror",System,cellIndex++,mirrorMat,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1019 -1007 -1004 -201");
      makeCell("MirrorVoid",System,cellIndex++,voidMat,0.0,Out);

      // yag screen

      Out=ModelSupport::getComposite(SMap,buildIndex,"2001 -2002 -2007");
      makeCell("YagScreen",System,cellIndex++,screenMat,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1005 -1006 2001 -2002 2007 (-2000:-2017) (-1003:-1009)");
      makeCell("YagHolder",System,cellIndex++,screenHolderMat,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1005 -1006 -1009 2002 (1003:2002) -1004 1002 ");
      makeCell("YagVoid",System,cellIndex++,voidMat,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1005 -1006 1002 2001 -2002 2017 2000 ");
       makeCell("YagVoid",System,cellIndex++,voidMat,0.0,Out);

      
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"1002 (2001:1003) -1004 1005 -1006 -201 ");
      addOuterSurf("Payload",Out);      
      
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
YagScreen::setBeamAxis(const attachSystem::FixedComp& FC,
		       const long int sIndex)
  /*!
    Set the screen centre
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
{
  ELog::RegMethod RegA("YagScreen","setBeamAxis(FC)");

  beamAxis=Geometry::Line(FC.getLinkPt(sIndex),
			  FC.getLinkAxis(sIndex));


  return;
}

void
YagScreen::setBeamAxis(const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& Axis)
  /*!
    Set the screen centre
    \param Org :: Origin point for line
    \param Axis :: Axis of line
  */
{
  ELog::RegMethod RegA("YagScreen","setBeamAxis(Vec3D)");

  beamAxis=Geometry::Line(Org,Axis);
  return;
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

  if (!isActive("side"))
    ELog::EM<<"Set pipeSide surface to simplify outer rule of YagScreen"
	    <<ELog::endWarn;
  if (!isActive("front"))
    ELog::EM<<"Set pipeFront surface to simplify outer rule of YagScreen"
	    <<ELog::endWarn;

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
