/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/M1DetailGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <stack>
#include <set>
#include <map>
#include <string>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "M1DetailGenerator.h"

namespace setVariable
{

M1DetailGenerator::M1DetailGenerator() :
  mWidth(6.0),mHeight(6.0),        
  mLength(37.0),mSlotXStep(4.5),     
  mSlotWidth(1.0),mSlotDepth(0.90),     
  mPipeXStep(2.17),mPipeYStep(2.0),
  mPipeZStep(1.8),mPipeSideRadius(0.125),
  mPipeWallThick(0.1),
  mPipeBaseLen(2.1),mPipeBaseRadius(0.25),
  mPipeOuterLen(1.5),mPipeOuterRadius(0.30),
  mNWater(11),mWaterLength(32.0),
  mWaterWidth(0.1),mWaterHeight(0.5),
  mWaterDrop(0.1),mWaterGap(0.1),

  fBaseGap(0.1),fBaseThick(1.4),fBaseWidth(5.97),  
  fBaseHeight(6.2),fCubePlateThick(1.1),
  fCubeThick(3.6),fCubeWidth(5.0),  
  fCubeHeight(4.4),fCubeSideWall(0.9),
  fCubeBaseWall(1.0),fInnerThick(1.2),
  fInnerHeight(2.0),fInnerWidth(2.8),
  fPipeYStep(1.8),fPipeLength(1.6),
  fPipeRadius(0.4),fPipeWall(0.1),
  
  sVOffset(0.8),sLength(26.5),sXOut(7.5),
  sThick(0.1),sEdge(1.1),sRadius(1.0),
  scLen(3.0),

  bLength(38.9),bClearGap(0.2),
  bBackThick(0.5),bMainThick(0.3),
  bExtentThick(0.4),
  bCupHeight(1.8),bTopExtent(4.2),
  bBaseExtent(2.1),bVoidExtra(3.5),
  bVoidBaseExtra(8.5),bVoidXExtra(4.5),
  
  clipYStep(7.7),clipLen(1.2),
  clipSiThick(0.2),clipAlThick(0.4),
  clipExtent(0.9),standoffRadius(0.5),
  standPts({
    Geometry::Vec3D(0.0,-7.5,-1.0),
    Geometry::Vec3D(0.0,0.0,0.0),
    Geometry::Vec3D(0.0,7.5,1.0)
  }),

  bFrontSupportThick(1.0),bFrontSupportCut(2.0),
  bFrontSupportZCut(0.2),

  bRingYStep(19.30),
  bRingOuterThick(0.3),bRingOuterLength(0.2),
  bRingInnerYStep(0.3),
  bRingInnerThick(1.2),bRingInnerLength(1.2),
  
  eXOut(1.98),eLength(38.0),eThick(0.1),eHeight(6.8),
  eEdge(2.40),eHoleRadius(1.18),
  eConnectLength(1.5),eConnectGap(1.3),
  eConnectThick(0.35),
  eBlockOffset(0.5),eBlockWidth(1.2),
  ePlateOffset(2.1),ePlateThick(0.2),
  ePlateHeight(5.2),ePipeRadius(0.3),
  ePipeThick(0.2),
  
  
  fBladeInRad(0.0),fBladeOutRad(2.0),
  fBladeThick(1.0),fBladeTopAngle(40.0),
  fBladeBaseAngle(60.0),fBladeBaseWidth(22.0),
  fBladeBaseHeight(5.0),fBladeFullHeight(12.0),  
  
  mirrorMat("Silicon300K"),waterMat("H2O"),
  supportMat("Aluminium"),springMat("Aluminium"),
  clipMat("Aluminium"),
  electronMat("Aluminium"),
  ringMat("Copper"),pipeMat("Aluminium"),
  outerMat("Aluminium"),frontMat("Copper"),
  
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}


M1DetailGenerator::~M1DetailGenerator() 
 /*!
   Destructor
 */
{}

void
M1DetailGenerator::makeSupport(FuncDataBase& Control,
			       const std::string& keyName) const
  /*!
    Create the variables for the base support
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeSupport");


  Control.addVariable(keyName+"SupVOffset",sVOffset);
  Control.addVariable(keyName+"SupLength",sLength);
  Control.addVariable(keyName+"SupXOut",sXOut);
  Control.addVariable(keyName+"SupThick",sThick);
  
  Control.addVariable(keyName+"SupEdge",sEdge);
  Control.addVariable(keyName+"SupHoleRadius",sRadius);

  Control.addVariable(keyName+"SpringConnectLen",scLen);
  
  Control.addVariable(keyName+"SupportMat",supportMat);
  Control.addVariable(keyName+"SpringMat",springMat);

  return;
}

void
M1DetailGenerator::makeFrontPlate(FuncDataBase& Control,
				  const std::string& keyName) const
  /*!
    Create the variables for the base support
    \param Control :: Function data base
    \param keyName :: pre-name for mirror+plate
   */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeFrontPlate");
    
  Control.addVariable(keyName+"BaseThick",fBaseThick);
  Control.addVariable(keyName+"BaseWidth",fBaseWidth);
  Control.addVariable(keyName+"BaseHeight",fBaseHeight);
  Control.addVariable(keyName+"CubePlateThick",fCubePlateThick); // fro
  Control.addVariable(keyName+"CubeThick",fCubeThick);
  Control.addVariable(keyName+"CubeWidth",fCubeWidth);
  Control.addVariable(keyName+"CubeHeight",fCubeHeight);
  Control.addVariable(keyName+"CubeSideWall",fCubeSideWall);
  Control.addVariable(keyName+"CubeBaseWall",fCubeBaseWall);
  Control.addVariable(keyName+"InnerThick",fInnerThick);
  Control.addVariable(keyName+"InnerHeight",fInnerHeight);
  Control.addVariable(keyName+"InnerWidth",fInnerWidth);
  Control.addVariable(keyName+"PipeYStep",fPipeYStep);
  Control.addVariable(keyName+"PipeLength",fPipeLength);
  Control.addVariable(keyName+"PipeRadius",fPipeRadius);
  Control.addVariable(keyName+"PipeWall",fPipeWall);


  Control.addVariable(keyName+"Mat",frontMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;
}
  
void
M1DetailGenerator::makeBackPlate(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Create the variables for the base support
    \param Control :: Function data base
    \param keyName :: Main keyname
   */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeBackPlate");

  Control.addVariable(keyName+"Length",bLength);
  Control.addVariable(keyName+"ClearGap",bClearGap);
  Control.addVariable(keyName+"BackThick",bBackThick);
  Control.addVariable(keyName+"MainThick",bMainThick);
  Control.addVariable(keyName+"CupHeight",bCupHeight);
  Control.addVariable(keyName+"TopExtent",bTopExtent);
  Control.addVariable(keyName+"ExtentThick",bExtentThick);
  Control.addVariable(keyName+"BaseExtent",bBaseExtent);
  Control.addVariable(keyName+"VoidExtra",bVoidExtra);
  Control.addVariable(keyName+"VoidBaseExtra",bVoidBaseExtra);
  Control.addVariable(keyName+"VoidXExtra",bVoidXExtra);

  Control.addVariable(keyName+"FrontPlateGap",fBaseGap);
  Control.addVariable(keyName+"FrontPlateWidth",fBaseWidth);
  Control.addVariable(keyName+"FrontPlateHeight",fBaseHeight);
  
  Control.addVariable(keyName+"BaseMat",supportMat);
  Control.addVariable(keyName+"FrontMat",frontMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;
}

void
M1DetailGenerator::makeConnectors(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Build the variables for connectors between the clamp and the mirror
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeConnnectors");

  Control.addVariable(keyName+"ClipYStep",clipYStep);
  Control.addVariable(keyName+"ClipLen",clipLen);
  Control.addVariable(keyName+"ClipSiThick",clipSiThick);
  Control.addVariable(keyName+"ClipAlThick",clipAlThick);
  Control.addVariable(keyName+"ClipExtent",clipExtent);

  Control.addVariable(keyName+"StandoffRadius",standoffRadius);
  Control.addVariable(keyName+"NStandoff",standPts.size());
  for(size_t i=0;i<standPts.size();i++)
    {
      Control.addVariable(keyName+"StandoffPt"+std::to_string(i),
			  standPts[i]);
    }
  
  Control.addVariable(keyName+"ClipMat",clipMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;
}

void
M1DetailGenerator::makeRingSupport(FuncDataBase& Control,
				   const std::string& keyName) const
  /*!
    
   */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeRingSupport");

  for(const std::string& key : {"A","B"})
    {
      const std::string kName=keyName+key;
      Control.addVariable(kName+"YStep",bRingYStep);
      Control.addVariable(kName+"OuterThick",bRingOuterThick);
      Control.addVariable(kName+"OuterLength",bRingOuterLength);
      Control.addVariable(kName+"InnerThick",bRingInnerThick);
      Control.addVariable(kName+"InnerLength",bRingInnerLength);
      Control.addVariable(kName+"InnerYStep",bRingInnerYStep);
      
      Control.addVariable(kName+"Mat",ringMat);
      Control.addVariable(kName+"VoidMat",voidMat);
    }
  Control.addVariable(keyName+"BPreXAngle",180.0);
  
  return;
}


void
M1DetailGenerator::makeOuterSupport(FuncDataBase& Control,
				    const std::string& keyName) const
  /*!
    Build the outer supports on the back plate
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeOuterSupport");

  /*
  Control.addVariable(keyName+"FrontSupportThick",bFrontSupportThick);
  Control.addVariable(keyName+"FrontSupportCut",bFrontSupportCut);
  Control.addVariable(keyName+"FrontSupportZCut",bFrontSupportZCut);
  */

 /*
  Control.addVariable(keyName+"RingYPos",bRingYPos);
  Control.addVariable(keyName+"RingThick",bRingThick);
  Control.addVariable(keyName+"RingGap",bRingGap);
  Control.addVariable(keyName+"RingClampThick",bRingClampThick);
  Control.addVariable(keyName+"RingBackPt",bRingBackPt);
  Control.addVariable(keyName+"RingTopPt",bRingTopPt);
  */
  return;
}

void
M1DetailGenerator::makeFrame(FuncDataBase& Control,
			     const std::string& keyName) const
  /*!
    Build the outer frame support
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeFrame");

  Control.addVariable(keyName+"BladeInRad",fBladeInRad);
  Control.addVariable(keyName+"BladeOutRad",fBladeOutRad);
  Control.addVariable(keyName+"BladeThick",fBladeThick);
  Control.addVariable(keyName+"BladeTopAngle",fBladeTopAngle);
  Control.addVariable(keyName+"BladeBaseAngle",fBladeBaseAngle);
  Control.addVariable(keyName+"BladeBaseWidth",fBladeBaseWidth);
  Control.addVariable(keyName+"BladeBaseHeight",fBladeBaseHeight);
  Control.addVariable(keyName+"BladeFullHeight",fBladeFullHeight);
  
  Control.addVariable(keyName+"SupportMat",supportMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  
  return;
}

void
M1DetailGenerator::makeElectronShield(FuncDataBase& Control,
				      const std::string& keyName) const
  /*!
    Build the electron shield
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeElectronShield");

  Control.addVariable(keyName+"ElecXOut",eXOut);
  Control.addVariable(keyName+"ElecLength",eLength);
  Control.addVariable(keyName+"ElecHeight",eHeight);
  Control.addVariable(keyName+"ElecEdge",eEdge);
  Control.addVariable(keyName+"ElecHoleRadius",eHoleRadius);
  Control.addVariable(keyName+"ElecThick",eThick);

  
  Control.addVariable(keyName+"ConnectLength",eConnectLength);
  Control.addVariable(keyName+"ConnectGap",eConnectGap);
  Control.addVariable(keyName+"ConnectThick",eConnectThick);

  Control.addVariable(keyName+"BlockOffset",eBlockOffset);
  Control.addVariable(keyName+"BlockWidth",eBlockWidth);
  Control.addVariable(keyName+"PlateThick",ePlateThick);
  Control.addVariable(keyName+"PlateOffset",ePlateOffset);
  Control.addVariable(keyName+"PlateHeight",ePlateHeight);

  Control.addVariable(keyName+"PipeRadius",ePipeRadius);
  Control.addVariable(keyName+"PipeThick",ePipeThick);
  
  Control.addVariable(keyName+"ElectronMat",electronMat);

  Control.addVariable(keyName+"SupportMat",supportMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  
  return;
}
  
void
M1DetailGenerator::makeCrystal(FuncDataBase& Control,
			       const std::string& cryName,
			       const double zStep,
			       const double theta) const 
  /*!
    Build the variables for the general crystal.
    The crystals are built generated in the centre of
    the viewed face.
    \param Control :: Database
    \param cryName :: extra name for crystal
    \param zStep :: displacement from MLM vessel white beam centre
    \param theta :: Angle to this crystal to the beam
    \param mirrorCenter :: Distance from 
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeCrystal");

  Control.addVariable(cryName+"ZStep",zStep);

  Control.addVariable(cryName+"ZAngle",theta);
  Control.addVariable(cryName+"Theta",theta);
  Control.addVariable(cryName+"Phi",0.0);
  
  Control.addVariable(cryName+"Width",mWidth);
  Control.addVariable(cryName+"Height",mHeight);
  Control.addVariable(cryName+"Length",mLength);

  Control.addVariable(cryName+"SlotXStep",mSlotXStep);
  Control.addVariable(cryName+"SlotWidth",mSlotWidth);
  Control.addVariable(cryName+"SlotDepth",mSlotDepth);

  Control.addVariable(cryName+"PipeXStep",mPipeXStep);
  Control.addVariable(cryName+"PipeYStep",mPipeYStep);
  Control.addVariable(cryName+"PipeZStep",mPipeZStep);
  
  Control.addVariable(cryName+"PipeSideRadius",mPipeSideRadius);
  Control.addVariable(cryName+"PipeWallThick",mPipeWallThick);
  Control.addVariable(cryName+"PipeBaseLen",mPipeBaseLen);
  Control.addVariable(cryName+"PipeBaseRadius",mPipeBaseRadius);
  Control.addVariable(cryName+"PipeOuterLen",mPipeOuterLen);
  Control.addVariable(cryName+"PipeOuterRadius",mPipeOuterRadius);

  Control.addVariable(cryName+"NWaterChannel",mNWater);
  Control.addVariable(cryName+"WaterLength",mWaterLength);
  Control.addVariable(cryName+"WaterWidth",mWaterWidth);
  Control.addVariable(cryName+"WaterHeight",mWaterHeight);
  Control.addVariable(cryName+"WaterDrop",mWaterWidth);
  Control.addVariable(cryName+"WaterGap",mWaterWidth);
    
  Control.addVariable(cryName+"MirrorMat",mirrorMat);
  Control.addVariable(cryName+"WaterMat",waterMat);
  Control.addVariable(cryName+"PipeMat",pipeMat);
  Control.addVariable(cryName+"OuterMat",outerMat);
  Control.addVariable(cryName+"VoidMat",voidMat);

  return;
}


void
M1DetailGenerator::generateMirror(FuncDataBase& Control,
				  const std::string& keyName,
				  const double theta,
				  const double zStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","generateMirror");

  // guess of separation
  makeCrystal(Control,keyName+"Mirror",theta,zStep);
  makeFrontPlate(Control,keyName+"FPlate");
  makeBackPlate(Control,keyName+"CClamp");
  makeElectronShield(Control,keyName+"ElectronShield");
  makeSupport(Control,keyName+"CClamp");
  makeConnectors(Control,keyName+"Connect");
  makeFrame(Control,keyName+"Frame");
  makeRingSupport(Control,keyName+"Ring");
  makeOuterSupport(Control,keyName+"CClamp");
  
  return;

}
  
}  // NAMESPACE setVariable
