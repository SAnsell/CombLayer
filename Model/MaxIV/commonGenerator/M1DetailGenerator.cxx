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

  sVOffset(0.8),sLength(26.5),sXOut(7.5),
  sThick(0.1),sEdge(1.1),sRadius(1.0),
  scLen(3.0),

  bLength(38.9),bClearGap(0.2),
  bBackThick(0.5),bMainThick(0.3),
  bExtentThick(0.4),
  bCupHeight(1.8),bTopExtent(4.2),
  bBaseExtent(2.1),bOuterVaneThick(0.8),
  bInnerVaneThick(0.4),
  
  clipYStep(7.7),clipLen(1.2),
  clipSiThick(0.2),clipAlThick(0.4),
  clipExtent(0.9),standoffRadius(0.5),
  standPts({
    Geometry::Vec3D(0.0,-7.5,-1.0),
    Geometry::Vec3D(0.0,0.0,0.0),
    Geometry::Vec3D(0.0,7.5,1.0)
  }),
  
  eXOut(7.98),eLength(38.0),eThick(0.1),eHeight(6.8),
  eEdge(1.03),eHoleRadius(1.18),
  
  mirrorMat("Silicon300K"),waterMat("H2O"),
  supportMat("Aluminium"),springMat("Aluminium"),
  clipMat("Aluminium"),
  electronMat("Aluminium"),pipeMat("Aluminium"),
  outerMat("Aluminium"),voidMat("Void")
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

  Control.addVariable(keyName+"ElecXOut",eXOut);
  Control.addVariable(keyName+"ElecLength",eLength);
  Control.addVariable(keyName+"ElecHeight",eHeight);
  Control.addVariable(keyName+"ElecEdge",eEdge);
  Control.addVariable(keyName+"ElecHoleRadius",eHoleRadius);
  Control.addVariable(keyName+"ElecThick",eThick);

  Control.addVariable(keyName+"ElectronMat",electronMat);

  return;
}

void
M1DetailGenerator::makeBackPlate(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Create the variables for the base support
    \param Control :: Function data base
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

  Control.addVariable(keyName+"OuterVaneThick",bOuterVaneThick);
  Control.addVariable(keyName+"InnerVaneThick",bInnerVaneThick);

  Control.addVariable(keyName+"BaseMat",supportMat);
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
  ELog::EM<<"M1 == "<<keyName<<ELog::endDiag;
  makeCrystal(Control,keyName+"Mirror",theta,zStep);
  makeBackPlate(Control,keyName+"CClamp");
  makeSupport(Control,keyName+"CClamp");
  makeConnectors(Control,keyName+"Connect");
  
  return;

}
  
}  // NAMESPACE setVariable
