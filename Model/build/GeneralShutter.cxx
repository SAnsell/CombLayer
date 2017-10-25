/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/GeneralShutter.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "GeneralShutter.h"

namespace shutterSystem
{

GeneralShutter::GeneralShutter(const size_t ID,const std::string& Key) : 
  TwinComp(Key+StrFunc::makeString(ID),8),ContainedComp(),
  shutterNumber(ID),baseName(Key),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName,20000)),
  cellIndex(surfIndex+1),populated(0),divideSurf(0),
  DPlane(0),closed(0),reversed(0),upperCell(0),
  lowerCell(0),innerVoidCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

GeneralShutter::GeneralShutter(const GeneralShutter& A) : 
  attachSystem::TwinComp(A),attachSystem::ContainedComp(A),
  shutterNumber(A.shutterNumber),baseName(A.baseName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  populated(A.populated),divideSurf(A.divideSurf),
  DPlane(A.DPlane),voidXoffset(A.voidXoffset),
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
  lowerCell(A.lowerCell),innerVoidCell(A.innerVoidCell)
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
      attachSystem::TwinComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      divideSurf=A.divideSurf;
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
      innerVoidCell=A.innerVoidCell;
    }
  return *this;
}

GeneralShutter::~GeneralShutter() 
  /*!
    Destructor
  */
{}

void
GeneralShutter::setGlobalVariables(const double IRad,
				   const double ORad,
				   const double floorZ,
				   const double topZ)
  /*!
    Sets external variables:
    \param IRad :: Inner [void radius]
    \param ORad :: Outer [Edge radius]
    \param floorZ :: Full extent of bulk shield
    \param topZ :: Full extent of bulk shield
  */
{
  innerRadius=IRad;
  outerRadius=ORad;
  totalHeight=topZ;
  totalDepth=floorZ;
  populated |= 2;
  return;
}
				
void
GeneralShutter::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("GeneralShutter","populate");
  const FuncDataBase& Control=System.getDataBase();

  voidXoffset=(Control.hasVariable("voidYoffset")) ? 
    Control.EvalVar<double>("voidYoffset") : 
    Control.EvalVar<double>("voidXoffset");
  // Global from shutter size:
  if (!(populated & 2))
    {
      innerRadius=Control.EvalVar<double>("bulkTorpedoRadius");
      outerRadius=Control.EvalVar<double>("bulkShutterRadius");  
      totalHeight=Control.EvalVar<double>("bulkRoof");
      totalDepth=Control.EvalVar<double>("bulkFloor");
      populated |= 2;
    }

  closed=Control.EvalDefPair<int>(keyName,baseName,"Closed",0);
  reversed=Control.EvalDefPair<int>(keyName,baseName,"Reversed",0);

  const std::string keyNum=
    StrFunc::makeString(keyName,shutterNumber+1);
  
  totalWidth=Control.EvalPair<double>(keyName,baseName,"Width");

  upperSteel=Control.EvalPair<double>(keyName,baseName,"UpperSteel");
  lowerSteel=Control.EvalPair<double>(keyName,baseName,"LowerSteel");

  shutterHeight=Control.EvalPair<double>(keyName,baseName,"Height");
  shutterDepth=Control.EvalPair<double>(keyName,baseName,"Depth");
  voidZOffset=Control.EvalDefPair<double>(keyName,baseName,"VoidZOffset",0.0);
  centZOffset=Control.EvalDefPair<double>(keyName,baseName,"centZOffSet",0.0);
  openZShift=Control.EvalDefPair<double>(keyName,baseName,"OpenZShift",0.0);
  closedZShift=openZShift-
    Control.EvalPair<double>(keyName,baseName,"ClosedZOffset");

  voidDivide=Control.EvalPair<double>(keyName,baseName,"VoidDivide");
  voidHeight=Control.EvalPair<double>(keyName,baseName,"VoidHeight");
  voidHeightInner=Control.EvalPair<double>(keyName,baseName,"VoidHeightInner");
  voidWidthInner=Control.EvalPair<double>(keyName,baseName,"VoidWidthInner");
  voidHeightOuter=Control.EvalPair<double>(keyName,baseName,"VoidHeightOuter");
  voidWidthOuter=Control.EvalPair<double>(keyName,baseName,"VoidWidthOuter");
  xyAngle=Control.EvalPair<double>(keyName,baseName,"XYAngle");
  zAngle=Control.EvalDefPair<double>(keyName,baseName,"ZAngle",0.0);

  shutterMat=ModelSupport::EvalMat<int>
    (Control,keyName+"SteelMat",baseName+"SteelMat");

  // Construct the clearance gaps
  clearGap=Control.EvalPair<double>(keyName,baseName,"ClearGap");
  clearBoxStep=Control.EvalPair<double>(keyName,baseName,"ClearBoxStep");
  clearBoxLen=Control.EvalPair<double>(keyName,baseName,"ClearBoxLen");
  const size_t NStep= Control.EvalPair<size_t>
    (keyName,baseName,"ClearNStep");
  clearCent.clear();
  for(size_t i=0;i<NStep;i++)
    {
      const std::string SCent="ClearCent"+StrFunc::makeString(i);
      const double CD=Control.EvalPair<double>(keyName,baseName,SCent);
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


  populated|=1;
  return;
}

void
GeneralShutter::createUnitVector(const attachSystem::FixedComp* FCPtr)
  /*!
    Create unit vectors for shutter along shutter direction
    \param FCPtr :: Previously defined Axis system [if present]
  */
{
  ELog::RegMethod RegA("GeneralShutter","createUnitVector");

  // Initial system down to TSA
  if (FCPtr)
    attachSystem::FixedComp::createUnitVector(*FCPtr);
  else
    {
      attachSystem::FixedComp::createUnitVector
	(Y*voidXoffset,
         Geometry::Vec3D(0,1,0),
         Geometry::Vec3D(0,0,-1),
         Geometry::Vec3D(-1,0,0));
    }
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
  // Now do rotation:
  XYAxis=Y;
  Geometry::Quaternion::calcQRotDeg(xyAngle,Z).rotate(XYAxis);
  // Create X 
  attachSystem::FixedComp::createUnitVector(Y*voidXoffset,
                                            XYAxis*Z,XYAxis,Z);

  BeamAxis=XYAxis;
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
  FixedComp::setConnect(0,frontPt,-XYAxis);
  FixedComp::setConnect(1,endPt,XYAxis);
    // TWIN STATUS [UN Modified direction]:
  bX=XYAxis*Z;
  bY=XYAxis;
  bZ=Z;

  bEnter=frontPt+Z*ZOffset;
  bExit=frontPt+bY*(outerRadius-innerRadius)+Z*ZOffset;
  // Now shift : frontPt:  
  const double zShift=(closed % 2) ? 
    closedZShift+ZOffset : openZShift+ZOffset;

  frontPt=Origin+XYAxis*innerRadius+Z*zShift;
  endPt=frontPt+BeamAxis*(outerRadius-innerRadius);

  // These are the modified output numbers:
  FixedComp::setConnect(6,frontPt,-XYAxis);
  FixedComp::setConnect(7,endPt,XYAxis);
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
GeneralShutter::setExternal(const int rInner,const int rOuter,
			    const int topP,const int baseP)
/*!
  Set the external surfaces
  \param rInner :: inner cylinder surface
  \param rOuter :: outer cylinder surface
  \param topP :: top plane
  \param baseP :: base plane
*/
{
  ELog::RegMethod RegA("GeneralShutter","setExternal");
  SMap.addMatch(surfIndex+7,rInner);
  SMap.addMatch(surfIndex+17,rOuter);
  SMap.addMatch(surfIndex+10,topP);
  SMap.addMatch(surfIndex+20,baseP);
  return;
}

void
GeneralShutter::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("GeneralShutter","createSurfaces");

  // Fixed Steel  
  ModelSupport::buildPlane(SMap,surfIndex+5,
			   Origin+Z*(totalHeight-upperSteel),Z);

  // Top blade [NOTE : BeamAxis]
  ModelSupport::buildPlane(SMap,surfIndex+15,
			   frontPt+Z*shutterHeight,Z);

  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+25,
   frontPt+Z*(voidZOffset+voidHeight/2.0+centZOffset),zSlope);
  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+26,
      frontPt-Z*(-voidZOffset+voidHeight/2.0-centZOffset),zSlope);

  // Lower Blade
  ModelSupport::buildPlane(SMap,surfIndex+16,
			   frontPt-Z*shutterDepth,Z);

  // Fixed Steel
  ModelSupport::buildPlane(SMap,surfIndex+6,
			   Origin-Z*(totalHeight-lowerSteel),Z);

  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+125,
       frontPt+Z*(voidZOffset+voidHeightInner/2.0+centZOffset),zSlope);
  
  // Inner cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+126,
	  frontPt-Z*(-voidZOffset+voidHeightInner/2.0-centZOffset),zSlope);

  // Outer cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+225,
	      frontPt+Z*(voidZOffset+voidHeightOuter/2.0+centZOffset),zSlope);
  
  // Outer cut [on flightline]
  ModelSupport::buildPlane(SMap,surfIndex+226,
	   frontPt-Z*(-voidZOffset+voidHeightOuter/2.0-centZOffset),zSlope);

  // Divide Plane
  ModelSupport::buildPlane(SMap,surfIndex+100,frontPt+Y*voidDivide,Y);

  // HORRIZONTAL:

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(totalWidth/2.0),X);

  // Clearance zone 
  ModelSupport::buildPlane(SMap,surfIndex+2003,
			   Origin-X*(clearGap+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+2004,
			   Origin+X*(clearGap+totalWidth/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+2013,
			   Origin-X*(clearBoxStep+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+2014,
			   Origin+X*(clearBoxStep+totalWidth/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+2023,
			   Origin-X*(clearBoxStep+clearGap+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+2024,
			   Origin+X*(clearBoxStep+clearGap+totalWidth/2.0),X);
  /// BOXES
  ModelSupport::buildPlane(SMap,surfIndex+2103,
			   Origin-X*(clearBoxStep+totalWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+2104,
			   Origin+X*(clearBoxStep+totalWidth/2.0),X);

  // boxes : Front/back planes
  int SN(surfIndex+2100);
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

  ModelSupport::buildPlane(SMap,surfIndex+13,
			   Origin-X*(voidWidthInner/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,
			   Origin+X*(voidWidthInner/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+113,
			   Origin-X*(voidWidthOuter/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+114,
			   Origin+X*(voidWidthOuter/2.0),X);
  return;
}

std::string
GeneralShutter::divideStr() const
  /*!
    Construct the divide string
    \return the +/- string form
   */
{
  std::ostringstream cx;
  if (xyAngle<0)
    cx<<" "<<divideSurf<<" ";
  else
    cx<<" "<<-divideSurf<<" ";
  return cx.str();
}
  
void
GeneralShutter::createCutUnit(Simulation& System,const std::string& ZUnit)
  /*!
    Create the cutouts for teh shutter clearance
    \param System :: Simulation part
    \param ZUnit :: Unit for +/- Z section
  */
{
  ELog::RegMethod RegA("GeneralShutter","createCutUnit");

  // CLEARANCE GAPS [Bulk Steel]:
  const std::string addUnit(ZUnit+" "+divideStr());
  std::string OutA,OutB;
  int SN(surfIndex+2000);
  for(size_t i=0;i<clearCent.size();i++)
    {
      if (!i)
	{
	  OutA=ModelSupport::getComposite(SMap,surfIndex,SN,
					  " (-3:4) 2003 -2004 7 -111M ")+addUnit;
	  OutB=ModelSupport::getComposite(SMap,surfIndex,SN,
					  " (-2003:2004) 2023 -2024 7 -111M ")+addUnit;
	}
      else
	{
	  OutA=ModelSupport::getComposite(SMap,surfIndex,SN,
					  "(-3:4) 2003 -2004 12M -111M")+addUnit;
	  OutB=ModelSupport::getComposite(SMap,surfIndex,SN,
					  "(-2003:2004) 2023 -2024 12M -111M ")+addUnit;
	}
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OutA));
      System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,OutB));

      // Insert blocks:
      OutA=ModelSupport::getComposite(SMap,surfIndex,SN,
				      "(-3:4) 2013 -2014 101M -102M ")+addUnit;
      System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,OutA));

      OutA=ModelSupport::getComposite(SMap,surfIndex,SN,
				      "(-101M:102M:-2013) 2023 -3 111M -112M ")+addUnit;
      OutB=ModelSupport::getComposite(SMap,surfIndex,SN,
				      "(-101M:102M:2014) -2024 4 111M -112M ")+addUnit;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OutA));
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OutB));

      SN+=100;
      
    }
  // END 
  OutA=ModelSupport::getComposite(SMap,surfIndex,SN,
				  "(-3:4) 2003 -2004 12M -17")+addUnit;
  OutB=ModelSupport::getComposite(SMap,surfIndex,SN,
				  "(-2003:2004) 2023 -2024 12M -17 ")+addUnit;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,OutA));
  System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,OutB));
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

  std::string Out;
  // Create divide string
  
  const std::string dSurf=divideStr();
  // top
  Out=ModelSupport::getComposite(SMap,surfIndex,"-10 5 2023 -2024 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));

  // void 
  Out=ModelSupport::getComposite(SMap,surfIndex,"-5 15 2023 -2024 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Bulk Steel
  Out=ModelSupport::getComposite(SMap,surfIndex,"-15 25 3 -4 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));
  upperCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,surfIndex,"-15 25");
  createCutUnit(System,Out);

  // Flightline
  if (voidDivide>0.0)
    {
      innerVoidCell=cellIndex;
      Out=ModelSupport::getComposite
	(SMap,surfIndex,"-125 126 13 -14 7 -100 ")+dSurf;
      MonteCarlo::Qhull IVHi(cellIndex++,0,0.0,Out);
      if (closed>1) IVHi.setImp(0);
      System.addCell(IVHi);

      Out=ModelSupport::getComposite
	(SMap,surfIndex,"-225 226 113 -114 100 -17 ")+dSurf;
      MonteCarlo::Qhull IVHo(cellIndex++,0,0.0,Out);
      if (closed>1) IVHo.setImp(0);
      System.addCell(IVHo);
      // Surrounder
      Out=ModelSupport::getComposite(SMap,surfIndex,
		     "-25 26 (125 : -126 : -13 : 14) 3 -4 7 -100 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,surfIndex,
	     "-25 26 (225 : -226 : -113 : 114) 3 -4 100 -17 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));
    }
  else
    {
      innerVoidCell=cellIndex;
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "-125 126 13 -14 7 -17 ")+dSurf;
      MonteCarlo::Qhull IVH(cellIndex++,0,0.0,Out);
      if (closed>1) IVH.setImp(0);
      System.addCell(IVH);

      Out=ModelSupport::getComposite(SMap,surfIndex,
	     " -25 26 (125 : -126 : -13 : 14) 3 -4 7 -17 ")+dSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));
    }  

  // Insert Clearance Gap
  Out=ModelSupport::getComposite(SMap,surfIndex,"-25 16");
  createCutUnit(System,Out);

  // Bulk Steel
  Out=ModelSupport::getComposite(SMap,surfIndex,"-26 16 3 -4 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));
  lowerCell=cellIndex-1;

  // Base void
  Out=ModelSupport::getComposite(SMap,surfIndex,"-16 6 2023 -2024 7 -17 ")
    +dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Base Steel
  Out=ModelSupport::getComposite(SMap,surfIndex,"-6 20 2023 -2024 7 -17 ")+dSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,shutterMat,0.0,Out));

  // Add exclude
  Out=ModelSupport::getComposite(SMap,surfIndex,"2023 -2024 ")+dSurf;
  addOuterSurf(Out);
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
  const int surfOffset(surfIndex+1000+
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
      TCube[2]=SMap.realSurf(surfIndex+25);
      LCube[3]=SMap.realSurf(surfIndex+26);
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
      LCube[4]=TCube[4]= SMap.realSurf(surfIndex+3);
      LCube[5]=TCube[5]= SMap.realSurf(surfIndex+4);
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
      System.addCell(MonteCarlo::Qhull(cellIndex,SB.matN,0.0,lx.str())); 
      SimProcess::registerOuter(System,lowerCell,cellIndex++);
    }
  if (SB.flag & 1) 
    {
      System.addCell(MonteCarlo::Qhull(cellIndex,SB.matN,0.0,tx.str())); 
      SimProcess::registerOuter(System,upperCell,cellIndex++);
    }
  return;
}

void
GeneralShutter::setDivide(const int D) 
  /*!
    Set the dividePlane
    \param D :: Plane [non-signed]
  */
{
  ELog::RegMethod RegA("GeneralShutter","setDivide");

  divideSurf=D;
  DPlane=(divideSurf) ? SMap.realPtr<Geometry::Plane>(divideSurf) : 0;
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
  window.push_back(SMap.realSurf(surfIndex+13));
  window.push_back(SMap.realSurf(surfIndex+14));
  window.push_back(SMap.realSurf(surfIndex+25));
  window.push_back(SMap.realSurf(surfIndex+26));

  Pt=frontPt+XYAxis*Dist+Z*voidZOffset;
  return SMap.realSurf(surfIndex+7);
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

  Out=ModelSupport::getComposite(SMap,surfIndex,"7 -100 ");
  FixedComp::addLinkSurf(0,Out);
  Out=ModelSupport::getComposite(SMap,surfIndex,"17 100 ");
  FixedComp::addLinkSurf(1,Out);

  FixedComp::addLinkSurf(2,SMap.realSurf(surfIndex+113));
  FixedComp::addLinkSurf(3,SMap.realSurf(surfIndex+114));
  FixedComp::addLinkSurf(4,SMap.realSurf(surfIndex+225));
  FixedComp::addLinkSurf(5,SMap.realSurf(surfIndex+226));


  FixedComp::setConnect(4,frontPt+Z*
			(voidZOffset+voidHeightOuter/2.0-centZOffset),-zSlope);
  FixedComp::setConnect(5,frontPt-Z*
			(-voidZOffset+voidHeightOuter/2.0-centZOffset),zSlope);


  return;
}


void
GeneralShutter::createAll(Simulation& System,
			  const double ZOffset,
			  const attachSystem::FixedComp* FCPtr)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param ZOffset :: Shift to the shutter creation
    \param FCPtr :: Previously valid axis system
  */
{
  ELog::RegMethod RegA("GeneralShutter","createAll");

  populate(System);
  createUnitVector(FCPtr);
  applyRotations(ZOffset);
  createSurfaces();
  createObjects(System);
  createBlocks(System); 
  createLinks();

  return;
}

}  // NAMESPACE shutterSystem


