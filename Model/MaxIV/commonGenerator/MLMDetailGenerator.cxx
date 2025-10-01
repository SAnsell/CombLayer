/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MLMDetailGenerator.cxx
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

#include "MLMDetailGenerator.h"

namespace setVariable
{

MLMDetailGenerator::MLMDetailGenerator() :
  gap(0.4),
  width(4.0),height(4.0),length(18.0),
  topSlotXStep(1.2),topSlotWidth(1.0),topSlotLength(16.0),
  topSlotDepth(0.2),
  baseWidth(7.2),baseLength(20.6),baseDepth(1.3),
  baseFrontHeight(0.9),baseBackHeight(1.3),
  baseInnerWidth(4.6),baseInnerBeamFaceLen(0.7),
  baseInnerOutFaceLen(1.3),baseBackSlotLen(12.1),
  baseOutSlotLen(2.3),
  wheelRadius(7.0),wheelOuterRadius(7.8),
  wheelHubRadius(2.5),wheelHeight(3.0),
  nSpokes(12),spokeThick(0.6),
  spokeCornerRadius(0.325),spokeCornerGap(0.1),

  radialLength(11.6),radialSupportLen(4.0),        
  radialTopGap(0.3),radialTopThick(0.8),      
  radialTopBeamWidth(1.6),radialTopOutWidth(5.6),   
  radialPlateThick(0.8),radialPlateLength(2.0),   
  radialPlateBeam(3.35),radialPlateXStep(1.5),
  radialSideWidth(9.0),radialSideBlock(2.1),     
  radialSideLift(1.13),radialSideFullWidth(20.0),
  radialSideBaseWidth(14.22),radialSideOutWidth(14.50),  

  radialSupportHeight(9.0),radialSupportOuterHeight(2.0),
  radialSupportInnerLift(1.38),
  
  radialBladeThick(0.6),radialBladeTopGap(6.07/4.0),   
  radialBladeBaseGap(12.67/4.0),

  WPlength(23.4),WPwidth(27.0),WPthick(2.8),
  WPridgeThick(1.2),WPridgeLen(1.7),
  WPoutSlotLength(8.0),
  WPoutSlotWidth(5.0),
  WPmidSlotXStep(-5.0),WPmidSlotLength(7.0),
  WPmidSlotWidth(4.5),

  WPdriveZClear(0.3),WPdriveThick(2.0), 
  WPdriveWidth(26.3),WPdriveLength(17.2),
  WPdriveTopLen(16.4),WPdriveBaseThick(0.8),
  WPdriveCutWidth(17.6),WPdriveCutLength(5.6),        
  WPdriveCutRadius(2.0),WPdriveCutRadLen(2.0),      
  
  WPbaseYStep(0.85),
  WPbaseThick(3.3),WPbaseWidth(28.4),WPbaseLength(25.2),
  WPbaseCutDepth(0.3),WPbaseCutFLen(2.5),WPbaseCutBLen(4.8),
  WPbaseCutWidth(26.4),WPbaseMidSlotWidth(4.7),
  WPbaseMidSlotHeight(1.65),
  WPbaseSideSlotEdge(2.0),WPbaseSideSlotHeight(1.3),
  WPbaseSideSlotWidth(6.1),
  
  plateMat("Stainless304"),mirrorMat("Silicon300K"),
  baseMat("Copper"),voidMat("Void")
  /*!
    Constructor and defaults
  */
{}


MLMDetailGenerator::~MLMDetailGenerator() 
 /*!
   Destructor
 */
{}

void
MLMDetailGenerator::makeRadialSupport(FuncDataBase& Control,
				      const std::string& rName,
				      const double xStep,
				      const double yStep) const
  /*!
    Build the variables for the radial supporting the first
    crystal
    \param Control :: Database
    \param rName :: extra name for radial
    \param xStep :: Step in x direction
    \param yStep :: Step in y direction
  */
    
{
  ELog::RegMethod RegA("MLMDetailGenerator","makeRadialSupport");

  Control.addVariable(rName+"XStep",xStep);
  Control.addVariable(rName+"YStep",yStep);
  
  Control.addVariable(rName+"Length",radialLength);
  Control.addVariable(rName+"SupportLen",radialSupportLen);
  
  Control.addVariable(rName+"TopGap",radialTopGap);
  Control.addVariable(rName+"TopThick",radialTopThick);
  Control.addVariable(rName+"TopBeamWidth",radialTopBeamWidth);
  Control.addVariable(rName+"TopOutWidth",radialTopOutWidth);
  
  Control.addVariable(rName+"PlateThick",radialPlateThick);
  Control.addVariable(rName+"PlateLength",radialPlateLength);
  Control.addVariable(rName+"PlateBeam",radialPlateBeam);
  Control.addVariable(rName+"PlateXStep",radialPlateXStep);
  
  Control.addVariable(rName+"SideWidth",radialSideWidth);
  Control.addVariable(rName+"SideBlock",radialSideBlock);
  Control.addVariable(rName+"SideLift",radialSideLift);
  Control.addVariable(rName+"SideFullWidth",radialSideFullWidth);
  Control.addVariable(rName+"SideBaseWidth",radialSideBaseWidth);
  Control.addVariable(rName+"SideOutWidth",radialSideOutWidth);

  Control.addVariable(rName+"SupportHeight",radialSupportHeight);
  Control.addVariable(rName+"SupportOuterHeight",radialSupportOuterHeight);
  Control.addVariable(rName+"SupportInnerLift",radialSupportInnerLift);
  
  Control.addVariable(rName+"BladeThick",radialBladeThick);
  Control.addVariable(rName+"BladeTopGap",radialBladeTopGap);
  Control.addVariable(rName+"BladeBaseGap",radialBladeBaseGap);

  
  Control.addVariable(rName+"BaseMat",baseMat);
  Control.addVariable(rName+"PlateMat",plateMat);
  Control.addVariable(rName+"VoidMat",voidMat);

  
  return;
}

void
MLMDetailGenerator::makeSupportWheel(FuncDataBase& Control,
				     const std::string& wheelName,
				     const double xStep,
				     const double yStep) const
  /*!
    Build the variables for the wheel supporting the second
    crystal
    \param Control :: Database
    \param wheelName :: extra name for wheel
    \param xStep :: Step in x direction
    \param yStep :: Step in y direction
  */
    
{
  ELog::RegMethod RegA("MLMDetailGenerator","makeSupportWheel");

  Control.addVariable(wheelName+"XStep",xStep);
  Control.addVariable(wheelName+"YStep",yStep);

  Control.addVariable(wheelName+"WheelRadius",wheelRadius);
  Control.addVariable(wheelName+"WheelOuterRadius",wheelOuterRadius);
  Control.addVariable(wheelName+"WheelHubRadius",wheelHubRadius);
  Control.addVariable(wheelName+"WheelHeight",wheelHeight);

  Control.addVariable(wheelName+"NSpokes",nSpokes);
  Control.addVariable(wheelName+"SpokeThick",spokeThick);
  Control.addVariable(wheelName+"SpokeCornerRadius",spokeCornerRadius);
  Control.addVariable(wheelName+"SpokeCornerGap",spokeCornerGap);
  
  Control.addVariable(wheelName+"Mat",baseMat);
  
  return;
}

void
MLMDetailGenerator::makeWheelPlate(FuncDataBase& Control,
				   const std::string& keyName,
				   const double xStep,
				   const double yStep) const
  /*!
    Build the variables for the wheel plate supporting the
    wheel.
    \param Control :: Database
    \param keyName :: extra name for plate
  */
    
{
  ELog::RegMethod RegA("MLMDetailGenerator","makeWheelPlate");

  Control.addVariable(keyName+"XStep",xStep);
  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Length",WPlength);
  Control.addVariable(keyName+"Width",WPwidth);
  Control.addVariable(keyName+"Thick",WPthick);
  Control.addVariable(keyName+"RidgeThick",WPridgeThick);
  Control.addVariable(keyName+"RidgeLen",WPridgeLen);
  Control.addVariable(keyName+"OutSlotWidth",WPoutSlotWidth);
  Control.addVariable(keyName+"OutSlotLength",WPoutSlotLength);
  Control.addVariable(keyName+"MidSlotXStep",WPmidSlotXStep);
  Control.addVariable(keyName+"MidSlotLength",WPmidSlotLength);
  Control.addVariable(keyName+"MidSlotWidth",WPmidSlotWidth);

  Control.addVariable(keyName+"DriveZClear",WPdriveZClear);
  Control.addVariable(keyName+"DriveThick",WPdriveThick);
  Control.addVariable(keyName+"DriveWidth",WPdriveWidth);
  Control.addVariable(keyName+"DriveLength",WPdriveLength);
  Control.addVariable(keyName+"DriveTopLen",WPdriveTopLen);
  Control.addVariable(keyName+"DriveBaseThick",WPdriveBaseThick);
  Control.addVariable(keyName+"DriveCutWidth",WPdriveCutWidth);
  Control.addVariable(keyName+"DriveCutLength",WPdriveCutLength);
  Control.addVariable(keyName+"DriveCutRadius",WPdriveCutRadius);
  Control.addVariable(keyName+"DriveCutRadLen",WPdriveCutRadLen);
  
  Control.addVariable(keyName+"BaseYStep",WPbaseYStep);
  Control.addVariable(keyName+"BaseThick",WPbaseThick);
  Control.addVariable(keyName+"BaseWidth",WPbaseWidth);
  Control.addVariable(keyName+"BaseLength",WPbaseLength);
  Control.addVariable(keyName+"BaseCutDepth",WPbaseCutDepth);
  Control.addVariable(keyName+"BaseCutFLen",WPbaseCutFLen);
  Control.addVariable(keyName+"BaseCutBLen",WPbaseCutBLen);
  Control.addVariable(keyName+"BaseCutWidth",WPbaseCutWidth);
  Control.addVariable(keyName+"BaseMidSlotWidth",WPbaseMidSlotWidth);
  Control.addVariable(keyName+"BaseMidSlotHeight",WPbaseMidSlotHeight);
  Control.addVariable(keyName+"BaseSideSlotEdge",WPbaseSideSlotEdge);
  Control.addVariable(keyName+"BaseSideSlotHeight",WPbaseSideSlotHeight);
  Control.addVariable(keyName+"BaseSideSlotWidth",WPbaseSideSlotWidth);

  Control.addVariable(keyName+"PlateMat",plateMat);
  Control.addVariable(keyName+"VoidMat",voidMat);

  return;
}

void
MLMDetailGenerator::makeCrystal(FuncDataBase& Control,
				const std::string& cryName,
				const bool rotFlag,
				const double xStep,
				const double yStep,
				const double theta) const
  /*!
    Build the variables for the general crystal.
    The crystals are built generated in the centre of
    the viewed face.
    \param Control :: Database
    \param cryName :: extra name for crystal
    \param rotFlag :: Crystal rotated 180 around Z
    \param xStep :: displacement from MLM vessel white beam centre
    \param yStep :: displacement from MLM vessel white beam centre
    \param theta :: Angle to this crystal to the beam
  */
{
  ELog::RegMethod RegA("MLMDetailGenerator","makeCrystal");

  const double rotAngle(rotFlag ? 180.0 : 0.0);

  Control.addVariable(cryName+"XStep",xStep);
  Control.addVariable(cryName+"YStep",yStep);
  Control.addVariable(cryName+"ZAngle",rotAngle+theta);
  
  Control.addVariable(cryName+"TopSlotXStep",topSlotXStep);
  Control.addVariable(cryName+"TopSlotWidth",topSlotWidth);
  Control.addVariable(cryName+"TopSlotDepth",topSlotDepth);
  Control.addVariable(cryName+"TopSlotLength",topSlotLength);

  Control.addVariable(cryName+"BaseWidth",baseWidth);
  Control.addVariable(cryName+"BaseLength",baseLength);
  Control.addVariable(cryName+"BaseDepth",baseDepth);
  Control.addVariable(cryName+"BaseFrontHeight",baseFrontHeight);
  Control.addVariable(cryName+"BaseBackHeight",baseBackHeight);
  Control.addVariable(cryName+"BaseInnerWidth",baseInnerWidth);
  Control.addVariable(cryName+"BaseInnerBeamFaceLen",baseInnerBeamFaceLen);
  Control.addVariable(cryName+"BaseInnerOutFaceLen",baseInnerOutFaceLen);
  Control.addVariable(cryName+"BaseBackSlotLen",baseBackSlotLen);
  Control.addVariable(cryName+"BaseOutSlotLen",baseOutSlotLen);
  
  Control.addVariable(cryName+"Width",width);
  Control.addVariable(cryName+"Height",height);
  Control.addVariable(cryName+"Length",length);

  Control.addVariable(cryName+"MirrorMat",mirrorMat);
  Control.addVariable(cryName+"BaseMat",baseMat);
  
  return;
}


void
MLMDetailGenerator::generateMono(FuncDataBase& Control,
				 const std::string& keyName,
				 const double thetaA,
				 const double thetaB) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param thetaA ::  angle
    
  */
{
  ELog::RegMethod RegA("MLMDetailGenerator","generateMono");

  // guess of separation
  const double xstalYStep(gap/tan(2.0*M_PI*thetaA/180.0));

  makeCrystal(Control,keyName+"XstalA",0,0.0,-xstalYStep/2.0,thetaA);
  makeCrystal(Control,keyName+"XstalB",1,-gap,xstalYStep/2.0,thetaB); 

  makeSupportWheel(Control,keyName+"BWheel",0.0,0.0);
  makeRadialSupport(Control,keyName+"Radial",0.0,0.0);
  makeWheelPlate(Control,keyName+"WheelPlate",0.0,3.0);
  
  return;
}
  
}  // NAMESPACE setVariable
