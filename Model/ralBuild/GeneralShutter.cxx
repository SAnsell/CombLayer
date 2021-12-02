/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/GeneralShutter.cxx
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "GeneralShutter.h"

namespace shutterSystem
{

GeneralShutter::GeneralShutter(const size_t ID,const std::string& Key) : 
  FixedGroup(Key+std::to_string(ID),"Main",8,"Beam",2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  shutterNumber(ID),baseName(Key),
  closed(0),reversed(0),upperCell(0),
  lowerCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

GeneralShutter::GeneralShutter(const GeneralShutter& A) : 
  attachSystem::FixedGroup(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::ExternalCut(A),
  shutterNumber(A.shutterNumber),baseName(A.baseName),
  voidXoffset(A.voidXoffset),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  totalHeight(A.totalHeight),totalDepth(A.totalDepth),
  totalWidth(A.totalWidth),upperSteel(A.upperSteel),
  lowerSteel(A.lowerSteel),shutterHeight(A.shutterHeight),
  shutterDepth(A.shutterDepth),voidZOffset(A.voidZOffset),
  centZOffset(A.centZOffset),closedZShift(A.closedZShift),
  openZShift(A.openZShift),voidDivide(A.voidDivide),
  voidHeight(A.voidHeight),voidHeightInner(A.voidHeightInner),
  voidWidthInner(A.voidWidthInner),
  voidHeightOuter(A.voidHeightOuter),
  voidWidthOuter(A.voidWidthOuter),clearGap(A.clearGap),
  clearBoxStep(A.clearBoxStep),clearBoxLen(A.clearBoxLen),
  clearCent(A.clearCent),xyAngle(A.xyAngle),zAngle(A.zAngle),
  shutterMat(A.shutterMat),closed(A.closed),
  reversed(A.reversed),SBlock(A.SBlock),XYAxis(A.XYAxis),
  BeamAxis(A.BeamAxis),zSlope(A.zSlope),targetPt(A.targetPt),
  frontPt(A.frontPt),endPt(A.endPt),upperCell(A.upperCell),
  lowerCell(A.lowerCell)
  /*!
    Copy constructor
    \param A :: GeneralShutter to copy
  */
{}

GeneralShutter&
GeneralShutter::operator=(const GeneralShutter& A)
  /*!
    Assignment operator
    \param A :: GeneralShutter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedGroup::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      voidXoffset=A.voidXoffset;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      totalHeight=A.totalHeight;
      totalDepth=A.totalDepth;
      totalWidth=A.totalWidth;
      upperSteel=A.upperSteel;
      lowerSteel=A.lowerSteel;
      shutterHeight=A.shutterHeight;
      shutterDepth=A.shutterDepth;
      voidZOffset=A.voidZOffset;
      centZOffset=A.centZOffset;
      closedZShift=A.closedZShift;
      openZShift=A.openZShift;
      voidDivide=A.voidDivide;
      voidHeight=A.voidHeight;
      voidHeightInner=A.voidHeightInner;
      voidWidthInner=A.voidWidthInner;
      voidHeightOuter=A.voidHeightOuter;
      voidWidthOuter=A.voidWidthOuter;
      clearGap=A.clearGap;
      clearBoxStep=A.clearBoxStep;
      clearBoxLen=A.clearBoxLen;
      clearCent=A.clearCent;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      shutterMat=A.shutterMat;
      closed=A.closed;
      reversed=A.reversed;
      SBlock=A.SBlock;
      XYAxis=A.XYAxis;
      BeamAxis=A.BeamAxis;
      zSlope=A.zSlope;
      targetPt=A.targetPt;
      frontPt=A.frontPt;
      endPt=A.endPt;
      upperCell=A.upperCell;
      lowerCell=A.lowerCell;
    }
  return *this;
}

GeneralShutter::~GeneralShutter() 
  /*!
    Destructor
  */
{}
				
void
GeneralShutter::populate(const FuncDataBase& Control)
/*!
  Populate all the variables
  \param Control :: Database to us
*/
{
  ELog::RegMethod RegA("GeneralShutter","populate");

  voidXoffset=Control.EvalPair<double>("voidYoffset","voidXoffset");

  innerRadius=Control.EvalTail<double>(keyName,baseName,"InnerRadius");
  outerRadius=Control.EvalTail<double>(keyName,baseName,"OuterRadius");
  totalHeight=Control.EvalTail<double>(keyName,baseName,"TotalHeight");
  totalDepth=Control.EvalTail<double>(keyName,baseName,"TotalDepth");

  closed=Control.EvalDefTail<int>(keyName,baseName,"Closed",0);
  reversed=Control.EvalDefTail<int>(keyName,baseName,"Reversed",0);

  const std::string keyNum=keyName+std::to_string(shutterNumber+1);
  
  totalWidth=Control.EvalTail<double>(keyName,baseName,"Width");

  upperSteel=Control.EvalTail<double>(keyName,baseName,"UpperSteel");
  lowerSteel=Control.EvalTail<double>(keyName,baseName,"LowerSteel");

  shutterHeight=Control.EvalTail<double>(keyName,baseName,"Height");
  shutterDepth=Control.EvalTail<double>(keyName,baseName,"Depth");
  voidZOffset=Control.EvalDefTail<double>(keyName,baseName,"VoidZOffset",0.0);
  centZOffset=Control.EvalDefTail<double>(keyName,baseName,"centZOffSet",0.0);
  openZShift=Control.EvalDefTail<double>(keyName,baseName,"OpenZShift",0.0);
  closedZShift=openZShift-
    Control.EvalTail<double>(keyName,baseName,"ClosedZOffset");

  voidDivide=Control.EvalTail<double>(keyName,baseName,"VoidDivide");
  voidHeight=Control.EvalTail<double>(keyName,baseName,"VoidHeight");
  voidHeightInner=Control.EvalTail<double>(keyName,baseName,"VoidHeightInner");
  voidWidthInner=Control.EvalTail<double>(keyName,baseName,"VoidWidthInner");
  voidHeightOuter=Control.EvalTail<double>(keyName,baseName,"VoidHeightOuter");
  voidWidthOuter=Control.EvalTail<double>(keyName,baseName,"VoidWidthOuter");
  xyAngle=Control.EvalTail<double>(keyName,baseName,"XYAngle");
  zAngle=Control.EvalDefTail<double>(keyName,baseName,"ZAngle",0.0);

  shutterMat=ModelSupport::EvalMat<int>
    (Control,keyName+"SteelMat",baseName+"SteelMat");

  // Construct the clearance gaps
  clearGap=Control.EvalTail<double>(keyName,baseName,"ClearGap");
  clearBoxStep=Control.EvalTail<double>(keyName,baseName,"ClearBoxStep");
  clearBoxLen=Control.EvalTail<double>(keyName,baseName,"ClearBoxLen");
  const size_t NStep= Control.EvalTail<size_t>
    (keyName,baseName,"ClearNStep");
  clearCent.clear();
  for(size_t i=0;i<NStep;i++)
    {
      const std::string SCent="ClearCent"+std::to_string(i);
      const double CD=Control.EvalTail<double>(keyName,baseName,SCent);
      clearCent.push_back(CD);
    }

  
  // This sets group of objects within the shutter
  // They require that each unit is fully defined and 
  // that requires 6 items
  shutterBlock Item;
  size_t flag(0);
  size_t itemNum(0);
  SBlock.clear();
  do 
    {
      if (flag==shutterBlock::Size)
	{
	  SBlock.push_back(Item);
	  itemNum++;
	  if (itemNum>10)
	    ELog::EM<<"Error with all variables in a block totally generic"
		    <<ELog::endErr;
	}
      for(flag=0;flag<shutterBlock::Size && 
	    Item.setFromControl(Control,"shutterBlock",
				shutterNumber+1,itemNum,flag);
	  flag++) ;

    } while (flag==shutterBlock::Size);


  return;
}

void
GeneralShutter::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create unit vectors for shutter along shutter direction
    \param FCPtr :: Previously defined Axis system [if present]
  */
{
  ELog::RegMethod RegA("GeneralShutter","createUnitVector");
  
  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  // Initial system down to TSA
  mainFC.createUnitVector(FC,sideIndex);
  beamFC.createUnitVector(FC,sideIndex);

  setDefault("Main","Beam");
  return;
}

void 
GeneralShutter::applyRotations(const double ZOffset)
  /*!
    Apply the rotations to the beamsystem 
    \param ZOffset :: additional offset if required
   */
{
  ELog::RegMethod RegA("GeneralShutter","applyRotations");

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  // Now do rotation:

  mainFC.setCentre(Y*voidXoffset);
  mainFC.applyAngleRotate(xyAngle,0);

  setDefault("Main","Beam");
			
  BeamAxis=mainFC.getY();
  XYAxis=BeamAxis;
  if (XYAxis.abs()<0.999) ELog::EM<<"ERRROR "<<BeamAxis<<ELog::endErr;
  
  zSlope=Z;
  Geometry::Quaternion::calcQRotDeg(zAngle,X).rotate(BeamAxis);
  Geometry::Quaternion::calcQRotDeg(zAngle,X).rotate(zSlope);
  // Set Front / Exit points
  // Special note [close == 2: open imp =-1]
  //              [close == 3: close imp =-1]

  targetPt=Origin+XYAxis*outerRadius;
  frontPt=Origin+XYAxis*innerRadius+Z*openZShift;
  endPt=frontPt+XYAxis*(outerRadius-innerRadius);
  

  // OUTPUT
  mainFC.setConnect(0,frontPt,-XYAxis);
  mainFC.setConnect(1,endPt,XYAxis);

  // TWIN STATUS [UN Modified direction]:
  beamFC.createUnitVector(frontPt+Z*ZOffset,XYAxis*Z,XYAxis,Z);
  beamFC.setConnect(0,frontPt+Z*ZOffset,-beamFC.getY());
  beamFC.setConnect(1,frontPt+beamFC.getY()*
		    (outerRadius-innerRadius)+Z*ZOffset,beamFC.getY());


  // Now shift : frontPt:  
  const double zShift=(closed % 2) ? 
    closedZShift+ZOffset : openZShift+ZOffset;

  frontPt=Origin+XYAxis*innerRadius+Z*zShift;
  endPt=frontPt+BeamAxis*(outerRadius-innerRadius);

  // These are the modified output numbers:
  mainFC.setConnect(6,frontPt,-XYAxis);
  mainFC.setConnect(7,endPt,XYAxis);

  setDefault("Main","Beam");
  
  return;
}

Geometry::Vec3D
GeneralShutter::getViewOrigin() const
  /*!
    Calculate view origin			
    \return Effective centre
  */
{
  // view effectively down void hence -ve void added
  const double zShift=(closed % 2) ? closedZShift : openZShift;
  return Origin+Z*(zShift-voidZOffset);
}


void
GeneralShutter::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("GeneralShutter","createSurfaces");

  // Divide:
  //  ModelSupport::buildPlane(SMap,buildIndex+10,Origin,Y);
  
  // Fixed Steel  
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin+Z*(totalHeight-upperSteel),Z);

  // Top blade [NOTE : BeamAxis]
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   frontPt+Z*shutterHeight,Z);

  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+25,
   frontPt+Z*(voidZOffset+voidHeight/2.0+centZOffset),zSlope);
  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+26,
      frontPt-Z*(-voidZOffset+voidHeight/2.0-centZOffset),zSlope);

  // Lower Blade
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   frontPt-Z*shutterDepth,Z);

  // Fixed Steel
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin-Z*(totalHeight-lowerSteel),Z);

  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+125,
       frontPt+Z*(voidZOffset+voidHeightInner/2.0+centZOffset),zSlope);
  
  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+126,
	  frontPt-Z*(-voidZOffset+voidHeightInner/2.0-centZOffset),zSlope);

  // Outer cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+225,
	      frontPt+Z*(voidZOffset+voidHeightOuter/2.0+centZOffset),zSlope);
  
  // Outer cut [on flightline]
  ModelSupport::buildPlane(SMap,buildIndex+226,
	   frontPt-Z*(-voidZOffset+voidHeightOuter/2.0-centZOffset),zSlope);

  // Forward Plane
  ModelSupport::buildPlane(SMap,buildIndex+200,frontPt-Y*(innerRadius/4.0),Y);
  // Divide Plane
  ModelSupport::buildPlane(SMap,buildIndex+100,frontPt+Y*voidDivide,Y);

  // HORRIZONTAL:

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(totalWidth/2.0),X);

  // Clearance zone 
  ModelSupport::buildPlane(SMap,buildIndex+2003,
			   Origin-X*(clearGap+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2004,
			   Origin+X*(clearGap+totalWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+2013,
			   Origin-X*(clearBoxStep+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2014,
			   Origin+X*(clearBoxStep+totalWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+2023,
			   Origin-X*(clearBoxStep+clearGap+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2024,
			   Origin+X*(clearBoxStep+clearGap+totalWidth/2.0),X);
  /// BOXES
  ModelSupport::buildPlane(SMap,buildIndex+2103,
			   Origin-X*(clearBoxStep+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2104,
			   Origin+X*(clearBoxStep+totalWidth/2.0),X);

  // boxes : Front/back planes
  int SN(buildIndex+2100);
  std::vector<double>::const_iterator vc;
  for(vc=clearCent.begin();vc!=clearCent.end();vc++)
    {
      ModelSupport::buildPlane(SMap,SN+1,
			       frontPt+Y*(*vc-clearBoxLen/2.0),Y);
      ModelSupport::buildPlane(SMap,SN+2,
			       frontPt+Y*(*vc+clearBoxLen/2.0),Y);
      ModelSupport::buildPlane(SMap,SN+11,
			       frontPt+Y*(*vc-clearGap-clearBoxLen/2.0),Y);
      ModelSupport::buildPlane(SMap,SN+12,
			       frontPt+Y*(*vc+clearBoxLen/2.0+clearGap),Y);
      SN+=100;
    }

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(voidWidthInner/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(voidWidthInner/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+113,
			   Origin-X*(voidWidthOuter/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+114,
			   Origin+X*(voidWidthOuter/2.0),X);
  return;
}
  
void
GeneralShutter::createCutUnit(Simulation& System,
			      const HeadRule& zHR)
  /*!
    Create the cutouts for the shutter clearance
    \param System :: Simulation part
    \param zHR :: Unit for +/- Z section
  */
{
  ELog::RegMethod RegA("GeneralShutter","createCutUnit");

  const HeadRule RInnerComp=ExternalCut::getComplementRule("RInner");
  const HeadRule ROuterHR=ExternalCut::getRule("ROuter");
  //  const HeadRule TPlane=ExternalCut::getRule("TopPlane");
  //  const HeadRule BPlane=ExternalCut::getRule("BasePlane");

  // CLEARANCE GAPS [Bulk Steel]:
  HeadRule OutA,OutB;
  int SN(buildIndex+2000);
  for(size_t i=0;i<clearCent.size();i++)
    {
      if (!i)
	{
	  OutA=ModelSupport::getHeadRule
	    (SMap,buildIndex,SN,"200 (-3:4) 2003 -2004 -111M ")
	    *RInnerComp*zHR;

	    OutB=ModelSupport::getHeadRule
	    (SMap,buildIndex,SN,"200 (-2003:2004) 2023 -2024 -111M ")
	    *RInnerComp*zHR;
	}
      else
	{
	  OutA=ModelSupport::getHeadRule
	    (SMap,buildIndex,SN,"200 (-3:4) 2003 -2004 12M -111M")*zHR;
	  OutB=ModelSupport::getHeadRule
	    (SMap,buildIndex,SN,"200 (-2003:2004) 2023 -2024 12M -111M ")*zHR;
	}
      makeCell("Void",System,cellIndex++,0,0.0,OutA);
      makeCell("Shutter",System,cellIndex++,shutterMat,0.0,OutB);

      // Insert blocks:
      OutA=ModelSupport::getHeadRule
	(SMap,buildIndex,SN,"200 (-3:4) 2013 -2014 101M -102M ")*zHR;
      makeCell("ShutterIBlock",System,cellIndex++,shutterMat,0.0,OutA);

      OutA=ModelSupport::getHeadRule
	(SMap,buildIndex,SN,"200 (-101M:102M:-2013) 2023 -3 111M -112M")*zHR; 
      OutB=ModelSupport::getHeadRule
	(SMap,buildIndex,SN,"200 (-101M:102M:2014) -2024 4 111M -112M")*zHR;
      
      makeCell("InsertVoid",System,cellIndex++,0,0.0,OutA);
      makeCell("InsertVoid",System,cellIndex++,0,0.0,OutB);
      SN+=100;
      
    }
  // END 
  OutA=ModelSupport::getHeadRule
    (SMap,buildIndex,SN,"200 (-3:4) 2003 -2004 12M")*zHR*ROuterHR;
  OutB=ModelSupport::getHeadRule
    (SMap,buildIndex,SN,"200 (-2003:2004) 2023 -2024 12M")*zHR*ROuterHR;
  makeCell("Void",System,cellIndex++,0,0.0,OutA);
  makeCell("Shutter",System,cellIndex++,shutterMat,0.0,OutB);
  return;
}

void
GeneralShutter::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("GeneralShutter","createObjects");

  // Create divide string
  const HeadRule RInnerComp=ExternalCut::getComplementRule("RInner");
  const HeadRule ROuterHR=ExternalCut::getRule("ROuter");
  const HeadRule TPlane=ExternalCut::getRule("TopPlane");
  const HeadRule BPlane=ExternalCut::getRule("BasePlane");

  HeadRule HR;
  // top
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 5 2023 -2024")*RInnerComp*ROuterHR*TPlane;
  makeCell("Top",System,cellIndex++,shutterMat,0.0,HR);

  // void 
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -5 15 2023 -2024")*RInnerComp*ROuterHR;
  makeCell("Void",System,cellIndex++,0,0.0,HR);


  // Bulk Steel
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -15 25 3 -4")*RInnerComp*ROuterHR;
  makeCell("UpperCell",System,cellIndex++,shutterMat,0.0,HR);  
  upperCell=cellIndex-1;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-15 25");
  createCutUnit(System,HR);

  // Flightline
  if (voidDivide>0.0)
    {
      innerVoidCell=cellIndex;
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-125 126 13 -14 -100 200")*RInnerComp;
      makeCell("InnerVoid",System,cellIndex++,0,0.0,HR);

      //if (closed>1) IVHi.setImp(0);


      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-225 226 113 -114 100")*ROuterHR;
      makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);

      //if (closed>1) IVHo.setImp(0);

      // Surrounder
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-25 26 (125 : -126 : -13 : 14) 3 -4 -100 200")
	*RInnerComp;
      makeCell("Surround",System,cellIndex++,shutterMat,0.0,HR);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-25 26 (225 : -226 : -113 : 114) 3 -4 100")*
	ROuterHR;
      makeCell("Surround",System,cellIndex++,shutterMat,0.0,HR);
    }
  else
    {
      innerVoidCell=cellIndex;
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"200 -125 126 13 -14")*RInnerComp*ROuterHR;
      MonteCarlo::Object IVH(cellIndex++,0,0.0,HR);
      if (closed>1) IVH.setImp(0);
      System.addCell(IVH);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"200 -25 26 (125:-126:-13:14) 3 -4")
	*RInnerComp*ROuterHR;
      makeCell("Surround",System,cellIndex++,shutterMat,0.0,HR);
    }  

  // Insert Clearance Gap
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-25 16");
  createCutUnit(System,HR);

  // Bulk Steel
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -26 16 3 -4")*RInnerComp*ROuterHR;
  makeCell("LowerCell",System,cellIndex++,shutterMat,0.0,HR);
  lowerCell=cellIndex-1;

  // Base void
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -16 6 2023 -2024")*RInnerComp*ROuterHR;
  makeCell("BaseVoid",System,cellIndex++,0,0.0,HR);

  // Base Steel
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -6 2023 -2024")*RInnerComp*ROuterHR*BPlane;
  makeCell("BaseSteel",System,cellIndex++,shutterMat,0.0,HR);

  // Add exclude
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"200 2023 -2024");
  addOuterSurf(HR);
  return;
}

void
GeneralShutter::createBlocks(Simulation& System)
  /*!
    Creates blocks for the top/lower block control
    \param System :: Simulation for cells.
  */
{
  ELog::RegMethod RegA("GeneralShutter","createAllBlocks");
  for(size_t i=0;i<SBlock.size();i++)
    createStopBlocks(System,i);
  return;
}


void
GeneralShutter::createStopBlocks(Simulation& System,const size_t BN)
  /*!
    Create the block within the shutter space
    \param System :: Simulation to update
    \param BN :: block number
  */
{
  ELog::RegMethod RegA("GeneralShutter","createStopBlocks");
  // if (shutterNumber==9) 
  //   return;

  if (BN>=SBlock.size())
    throw ColErr::IndexError<size_t>(BN,SBlock.size(),
				     " stopBlock size");
  const shutterBlock& SB=SBlock[BN];
  
  int TCube[6];      // 6-sides of a cube [Lower/Upper]
  int LCube[6];
  const int surfOffset(buildIndex+1000+
		       10*static_cast<int>(BN));

  // Front/Back (Y AXIS)
  ModelSupport::buildPlane(SMap,surfOffset+3,
			   frontPt+Y*(SB.centY-SB.length/2.0),Y);
  LCube[0]=TCube[0]=SMap.realSurf(surfOffset+3);
  ModelSupport::buildPlane(SMap,surfOffset+4,
			   frontPt+Y*(SB.centY+SB.length/2.0),Y);
  LCube[1]=TCube[1]=SMap.realSurf(surfOffset+4);

  // Z [view side]
  if (SB.edgeVGap>0)
    {
      ModelSupport::buildPlane(SMap,surfOffset+5,
	   frontPt+Z*(voidZOffset+voidHeightOuter/2.0+SB.edgeVGap),Z);
      TCube[2]=SMap.realSurf(surfOffset+5);

      ModelSupport::buildPlane(SMap,surfOffset+7,
	   frontPt-Z*(-voidZOffset+voidHeightOuter/2.0+SB.edgeVGap),Z);
      LCube[3]=SMap.realSurf(surfOffset+7);
    }
  else
    {
      TCube[2]=SMap.realSurf(buildIndex+25);
      LCube[3]=SMap.realSurf(buildIndex+26);
    }

  // Full height
  ModelSupport::buildPlane(SMap,surfOffset+6,
	    frontPt+Z*(voidZOffset+voidHeightOuter/2.0+SB.edgeVGap+SB.height),Z);
  TCube[3]=SMap.realSurf(surfOffset+6);

  ModelSupport::buildPlane(SMap,surfOffset+8,
      frontPt-Z*(-voidZOffset+voidHeightOuter/2.0+SB.edgeVGap+SB.height),Z);
  LCube[2]=SMap.realSurf(surfOffset+8);
  
  // X coordinate:
  if (std::abs(SB.edgeHGap)>Geometry::shiftTol)
    {
      ModelSupport::buildPlane(SMap,surfOffset+1,      
			       Origin-X*(totalWidth/2.0-SB.edgeHGap),X);
      LCube[4]=TCube[4]=SMap.realSurf(surfOffset+1);

      ModelSupport::buildPlane(SMap,surfOffset+2,      
			       Origin+X*(totalWidth/2.0-SB.edgeHGap),X);
      LCube[5]=TCube[5]=SMap.realSurf(surfOffset+2);
    }
  else 
    {
      LCube[4]=TCube[4]= SMap.realSurf(buildIndex+3);
      LCube[5]=TCube[5]= SMap.realSurf(buildIndex+4);
    }
  // ---------------------------
  // CREATE Inner objects:
  // ---------------------------

  std::ostringstream tx;  // top 
  std::ostringstream lx;  // lower
  int sign(1);
  for(size_t i=0;i<6;i++)
    {
      lx<<sign*LCube[i]<<" ";
      tx<<sign*TCube[i]<<" ";
      sign*=-1;
    }
  if (SB.flag & 2)          // Lower
    {
      System.addCell(MonteCarlo::Object(cellIndex,SB.matN,0.0,lx.str())); 
      SimProcess::registerOuter(System,lowerCell,cellIndex++);
    }
  if (SB.flag & 1) 
    {
      System.addCell(MonteCarlo::Object(cellIndex,SB.matN,0.0,tx.str())); 
      SimProcess::registerOuter(System,upperCell,cellIndex++);
    }
  return;
}

int
GeneralShutter::exitWindow(const double Dist,
			   std::vector<int>& window,
			   Geometry::Vec3D& Pt) const
  /*!
    Get the exit window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Surface
  */
{
  ELog::RegMethod RegA("GeneralShutter","exitWindow");
  window.clear();
  window.push_back(SMap.realSurf(buildIndex+13));
  window.push_back(SMap.realSurf(buildIndex+14));
  window.push_back(SMap.realSurf(buildIndex+25));
  window.push_back(SMap.realSurf(buildIndex+26));

  Pt=frontPt+XYAxis*Dist+Z*voidZOffset;
  return SMap.realSurf(buildIndex+7);
}

void
GeneralShutter::createLinks() 
  /*!
    Create link units : 
    Some set in unitVector
  */
{
  ELog::RegMethod RegA("GeneralShutter","createLinks");
  std::string Out;

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");

  const HeadRule RInnerComp=ExternalCut::getComplementRule("RInner");
  const HeadRule ROuterHR=ExternalCut::getRule("ROuter");
  
  const HeadRule dA=ModelSupport::getHeadRule(SMap,buildIndex,"-200");
  const HeadRule dB=ModelSupport::getHeadRule(SMap,buildIndex,"200");

  HeadRule HR;

  HR=dA*RInnerComp;
  mainFC.addLinkSurf(0,HR);
  
  HR=dB*ROuterHR;
  mainFC.addLinkSurf(1,HR);

  mainFC.addLinkSurf(2,SMap.realSurf(buildIndex+113));
  mainFC.addLinkSurf(3,SMap.realSurf(buildIndex+114));
  mainFC.addLinkSurf(4,SMap.realSurf(buildIndex+225));
  mainFC.addLinkSurf(5,SMap.realSurf(buildIndex+226));


  mainFC.setConnect(4,frontPt+Z*
			(voidZOffset+voidHeightOuter/2.0-centZOffset),-zSlope);
  mainFC.setConnect(5,frontPt-Z*
			(-voidZOffset+voidHeightOuter/2.0-centZOffset),zSlope);
  /// divide
  mainFC.nameSideIndex(6,"Divider");
  mainFC.setConnect(6,Origin,Y);
  mainFC.addLinkSurf(6,SMap.realSurf(buildIndex+200));


  return;
}


void
GeneralShutter::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param ZOffset :: Shift to the shutter creation
    \param FCPtr :: Previously valid axis system
  */
{
  ELog::RegMethod RegA("GeneralShutter","createAll");

  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  applyRotations(ZOffset);
  createSurfaces();
  createObjects(System);
  createBlocks(System); 
  createLinks();

  return;
}

}  // NAMESPACE shutterSystem


