/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: Linac/TDC.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "BlockZone.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Importance.h"
#include "Object.h"

#include "InjectionHall.h"
#include "TDCsegment.h"
#include "Segment1.h"
#include "Segment2.h"
#include "Segment3.h"
#include "Segment4.h"
#include "Segment5.h"
#include "Segment6.h"
#include "Segment7.h"
#include "Segment8.h"
#include "Segment9.h"
#include "Segment10.h"
#include "Segment11.h"
#include "Segment12.h"
#include "Segment13.h"

#include "Segment14.h"
#include "Segment15.h"
#include "Segment16.h"
#include "Segment17.h"
#include "Segment18.h"
#include "Segment19.h"
#include "Segment20.h"
#include "Segment21.h"
#include "Segment23.h"
#include "Segment24.h"
#include "Segment25.h"
#include "Segment26.h"
#include "Segment27.h"
#include "Segment28.h"
#include "Segment29.h"
#include "Segment30.h"
#include "Segment31.h"
#include "Segment32.h"
#include "Segment33.h"
#include "Segment34.h"
#include "Segment35.h"
#include "Segment36.h"
#include "Segment37.h"
#include "Segment38.h"
#include "Segment39.h"
#include "Segment40.h"
#include "Segment41.h"
#include "Segment42.h"
#include "Segment43.h"
#include "Segment44.h"
#include "Segment45.h"
#include "Segment46.h"
#include "Segment47.h"
#include "Segment48.h"
#include "Segment49.h"

#include "TDC.h"

namespace tdcSystem
{

TDC::TDC(const std::string& KN) :
  attachSystem::FixedOffset(KN,6),
  attachSystem::CellMap(),
  noCheck(0),pointCheck(0),
  injectionHall(new InjectionHall("InjectionHall")),
  SegMap
  ({
    { "Segment1",std::make_shared<Segment1>("L2SPF1") },
    { "Segment2",std::make_shared<Segment2>("L2SPF2") },
    { "Segment3",std::make_shared<Segment3>("L2SPF3") },
    { "Segment4",std::make_shared<Segment4>("L2SPF4") },
    { "Segment5",std::make_shared<Segment5>("L2SPF5") },
    { "Segment6",std::make_shared<Segment6>("L2SPF6") },
    { "Segment7",std::make_shared<Segment7>("L2SPF7") },
    { "Segment8",std::make_shared<Segment8>("L2SPF8") },
    { "Segment9",std::make_shared<Segment9>("L2SPF9") },
    { "Segment10",std::make_shared<Segment10>("L2SPF10") },
    { "Segment11",std::make_shared<Segment11>("L2SPF11") },
    { "Segment12",std::make_shared<Segment12>("L2SPF12") },
    { "Segment13",std::make_shared<Segment13>("L2SPF13") },
    { "Segment14",std::make_shared<Segment14>("TDC14") },
    { "Segment15",std::make_shared<Segment15>("TDC15") },
    { "Segment16",std::make_shared<Segment16>("TDC16") },
    { "Segment17",std::make_shared<Segment17>("TDC17") },
    { "Segment18",std::make_shared<Segment18>("TDC18") },
    { "Segment19",std::make_shared<Segment19>("TDC19") },
    { "Segment20",std::make_shared<Segment20>("TDC20") },
    { "Segment21",std::make_shared<Segment21>("TDC21") },
    { "Segment22",std::make_shared<Segment20>("TDC22") },
    { "Segment23",std::make_shared<Segment23>("TDC23") },
    { "Segment24",std::make_shared<Segment24>("TDC24") },
    { "Segment25",std::make_shared<Segment25>("TDC25") },
    { "Segment26",std::make_shared<Segment26>("TDC26") },
    { "Segment27",std::make_shared<Segment27>("TDC27") },
    { "Segment28",std::make_shared<Segment28>("TDC28") },
    { "Segment29",std::make_shared<Segment29>("TDC29") },
    { "Segment30",std::make_shared<Segment30>("SPF30") },
    { "Segment31",std::make_shared<Segment31>("SPF31") },
    { "Segment32",std::make_shared<Segment32>("SPF32") },
    { "Segment33",std::make_shared<Segment33>("SPF33") },
    { "Segment34",std::make_shared<Segment34>("SPF34") },
    { "Segment35",std::make_shared<Segment35>("SPF35") },
    { "Segment36",std::make_shared<Segment36>("SPF36") },
    { "Segment37",std::make_shared<Segment37>("SPF37") },
    { "Segment38",std::make_shared<Segment38>("SPF38") },
    { "Segment39",std::make_shared<Segment39>("SPF39") },
    { "Segment40",std::make_shared<Segment40>("SPF40") },
    { "Segment41",std::make_shared<Segment41>("SPF41") },
    { "Segment42",std::make_shared<Segment42>("SPF42") },
    { "Segment43",std::make_shared<Segment43>("SPF43") },
    { "Segment44",std::make_shared<Segment44>("SPF44") },
    { "Segment45",std::make_shared<Segment45>("SPF45") },
    { "Segment46",std::make_shared<Segment46>("SPF46") },
    { "Segment47",std::make_shared<Segment47>("SPF47") },
    { "Segment48",std::make_shared<Segment48>("SPF48") },
    { "Segment49",std::make_shared<Segment49>("SPF49") }

  } )
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ELog::RegMethod RegA("TDC","TDC");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(injectionHall);
  for(const auto& [ key, tdcItem ] : SegMap)
    OR.addObject(tdcItem);
}

TDC::~TDC()
  /*!
    Destructor
   */
{}


HeadRule
TDC::buildSurround(const FuncDataBase& Control,
		   const std::string& regionName,
		   const std::string& injectionPt)
  /*!
    Given a region name build a surround
    \param regionName :: region to used
    \param injectionPt :: link poin in the injection hall to use
  */
{
  ELog::RegMethod RegA("TDC","buildSurround");

  /// storeage
  static std::map<std::string,HeadRule> surroundMap;
  static int BI(buildIndex);   // keep regions in order and unique

  std::map<std::string,HeadRule>::const_iterator mc=
    surroundMap.find(regionName);

  if (mc==surroundMap.end())
    {
      const double outerLeft=Control.EvalVar<double>(regionName+"OuterLeft");
      const double outerRight=Control.EvalVar<double>(regionName+"OuterRight");
      const double outerTop=Control.EvalVar<double>(regionName+"OuterTop");
      const double outerFloor=
	Control.EvalDefVar<double>(regionName+"OuterFloor",-5e10); // large-neg

      const attachSystem::FixedOffsetUnit injFC
	(Control,regionName,*injectionHall,injectionPt);
      // rotation if needed in a bit
      const Geometry::Vec3D& Org=injFC.getCentre();
      const Geometry::Vec3D& InjectX=injFC.getX();
      const Geometry::Vec3D& InjectZ=injFC.getZ();

      std::string Out;

      ModelSupport::buildPlane(SMap,BI+3,Org-X*outerLeft,InjectX);
      ModelSupport::buildPlane(SMap,BI+4,Org+X*outerRight,InjectX);
      ModelSupport::buildPlane(SMap,BI+6,Org+Z*outerTop,InjectZ);
      if (outerFloor < -4e10)
	{
	  Out=ModelSupport::getComposite(SMap,BI," 3 -4 -6");
	  BI+=10;
	  return HeadRule(Out+injectionHall->getSurfString("Floor"));
	}

      ModelSupport::buildPlane(SMap,BI+5,Org-Z*outerFloor,InjectZ);
      Out=ModelSupport::getComposite(SMap,BI," 3 -4 5 -6");
      BI+=10;

      HeadRule OutHR(Out);
      surroundMap.emplace(regionName,OutHR);
      return OutHR;
    }

  return mc->second;
}

void
TDC::setVoidSpace(const Simulation& System,
		  const std::shared_ptr<attachSystem::BlockZone>& buildZone,
		  const std::string& voidName)
  /*!
    Add the void cells from the injection hall
    \param System :: simulation
    \param buildZone :: BuildZone
    \param :: Name of void space
  */
{
  ELog::RegMethod RegA("TDC","setVoidSpace");

  if (!voidName.empty())
    {
      const std::vector<int>& VCell=
	injectionHall->getCells(voidName);
      for(const int CN : VCell)
	{
	  if (originalSpaces.find(CN)==originalSpaces.end())
	    {
	      const MonteCarlo::Object* OPtr=System.findObject(CN);
	      originalSpaces.emplace(CN,OPtr->getHeadRule());
	    }
	}
      buildZone->addInsertCells(VCell);
    }
  return;
}


std::shared_ptr<attachSystem::BlockZone>
TDC::buildInnerZone(Simulation& System,
		    const std::string& segmentName,
		    const std::string& regionName)
  /*!
    Set the regional buildzone
    \param System :: Simulation
    \param regionName :: Zone name
  */
{
  ELog::RegMethod RegA("TDC","buildInnerZone");

  // FrontSurf : BackSurf : Cell : Cell(if not empty)
  typedef std::tuple<std::string,std::string,std::string,std::string> RTYPE;
  typedef std::map<std::string,RTYPE> RMAP;
  typedef std::map<std::string,std::string> EMAP;

  // front : back : Insert
  const static RMAP regZones
    ({
      {"l2spf",{"Front","#KlystronWall","LinearVoid","LWideVoid"}},
      {"l2spfTurn",{"KlystronWall","#MidWall","LWideVoid",""}},
      {"l2spfAngle",{"KlystronCorner","MidAngleWall","LWideVoid","LTVoid"}},
      {"tdcFront"  ,{"DoorEndWall","#TDCMid","SPFVoid","TVoidB"}},
      {"tdcMain"  ,{"TDCStart","#TDCMid","SPFVoid",""}},
      {"tdc"  ,{"TDCCorner","#TDCMid","SPFVoid","LongVoid"}},
      {"tdcShort", {"TDCAngleMid","#TDCMid","SPFVoid","LongVoid"}},
      {"spfLong"  ,{"TDCMid","#Back","LongVoid",""}},
      {"spfAngle"  ,{"TDCCorner","#TDCMid","SPFVoid","LongVoid"}},
      {"spf"  ,{"TDCCorner","#TDCMid","SPFVoid","LongVoid"}},
      {"spfFar"  ,{"TDCMid","#BackWallFront","LongVoid",""}}, // last cell with columns
      {"spfBehindBackWall"  ,{"BackWallBack","#FemtoMAXBack","C080016",""}} // cell behind back wall
    });
  //Extra surfaces to add to the main surround
  const static EMAP extraSUR
    ({
      {"l2spfAngle","#MidWall"}
    });

  const FuncDataBase& Control=System.getDataBase();

  RMAP::const_iterator rc=regZones.find(regionName);
  if (rc==regZones.end())
    throw ColErr::InContainerError<std::string>(regionName,"regionZones");
  EMAP::const_iterator ec=extraSUR.find(regionName);

  // try not to use front surf:
  const RTYPE& walls=rc->second;
  const std::string& frontSurfName=std::get<0>(walls);
  const std::string& backSurfName=std::get<1>(walls);
  const std::string& voidName=std::get<2>(walls);
  const std::string& voidNameB=std::get<3>(walls);

  if (bZone.find(segmentName)!=bZone.end())
    throw ColErr::InContainerError<std::string>
      (segmentName,"Segment Name exists");

  std::shared_ptr<attachSystem::BlockZone> buildZone=
    std::make_shared<attachSystem::BlockZone>(segmentName);

  buildZone->setFront(injectionHall->getSurfRules(frontSurfName));
  HeadRule surHR=buildSurround(Control,regionName,"Origin");
  if (ec!=extraSUR.end())
    surHR.addIntersection(injectionHall->getSurfRules(ec->second));

  buildZone->setSurround(surHR);
  buildZone->setMaxExtent(injectionHall->getSurfRules(backSurfName));

  setVoidSpace(System,buildZone,voidName);
  setVoidSpace(System,buildZone,voidNameB);

  auto [mc,successflag] =  bZone.emplace(segmentName,buildZone);
  return mc->second;
}

void
TDC::reconstructInjectionHall(Simulation& System)
  /*!
    Regenerate injection hall
    \param System :: simulatiion
  */
{
  ELog::RegMethod RegA("TDC","reconstructInjectionHall");
  // Make list of unique insert cells:
  std::set<int> CInsert;
  for(const auto& [name,bzPtr] : bZone)
    {
      const std::vector<int>& CVec=bzPtr->getInsertCells();
      for(const int CN : CVec)
	CInsert.insert(CN);
    }

  attachSystem::BlockZone BZvol;
  for(const int CN : CInsert)
    {
      HeadRule OuterVolume;
      bool initFlag(1);
      for(const auto& [name,bzPtr] : bZone)
	{
	  const std::vector<int>& CVec=bzPtr->getInsertCells();
	  if (std::find(CVec.begin(),CVec.end(),CN)!=CVec.end())
	    {
	      if (initFlag)
		{
		  BZvol=*(bzPtr);
		  initFlag=0;
		}
	      else if (!BZvol.merge(*bzPtr))
		{
		  OuterVolume.addIntersection(BZvol.getVolume().complement());
		  BZvol= *bzPtr;
		}
	    }
	}
      std::map<int,HeadRule>::iterator mc=
	originalSpaces.find(CN);

      if (mc==originalSpaces.end())
	throw ColErr::InContainerError<int>(CN,"BZone insertcell");

      HeadRule HROut=mc->second;
      HROut.addIntersection(BZvol.getVolume().complement());
      HROut.addIntersection(OuterVolume);

      MonteCarlo::Object* OPtr=System.findObject(CN);
      OPtr->procHeadRule(HROut);
    }



  return;
}

void
TDC::createAll(Simulation& System,
	       const attachSystem::FixedComp& FCOrigin,
	       const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FCOrigin :: Start origin
    \param sideIndex :: link point for origin
   */
{
  // For output stream
  ELog::RegMethod RControl("TDC","createAll");

  // this is requires to stop non-alphabetical segments
  // being built out of order
  static const std::vector<std::string> buildOrder
    ({
      "Segment1","Segment2","Segment3",
      "Segment4","Segment5","Segment6",
      "Segment7","Segment8","Segment9",
      "Segment10","Segment11","Segment12",
      "Segment13",
      "Segment14","Segment15","Segment16",
      "Segment17","Segment18","Segment19",
      "Segment20","Segment21","Segment22",
      "Segment23","Segment24","Segment25",
      "Segment26","Segment27","Segment28",
      "Segment29","Segment30","Segment31",
      "Segment32","Segment33","Segment34",
      "Segment35","Segment36","Segment37",
      "Segment38","Segment39","Segment40",
      "Segment41","Segment42","Segment43",
      "Segment44","Segment45","Segment46",
      "Segment47","Segment48","Segment49"
    });

  // buildZone : previous : firstVecIndex
  typedef std::tuple<std::string,std::string,size_t> LinkTYPE;
  static const std::map<std::string,LinkTYPE> segmentLinkMap
    ({
      {"Segment1",{"l2spf","",1}},
      {"Segment2",{"l2spf","Segment1",1}},
      {"Segment3",{"l2spf","Segment2",1}},
      {"Segment4",{"l2spf","Segment3",1}},
      {"Segment5",{"l2spfAngle","Segment4",1}},
      {"Segment6",{"l2spfAngle","Segment5",1}},
      {"Segment7",{"l2spfAngle","Segment6",1}},
      {"Segment8",{"l2spfAngle","Segment7",1}},
      {"Segment9",{"l2spfAngle","Segment8",1}},
      {"Segment10",{"l2spfAngle","Segment9",1}},
      {"Segment11",{"tdcFront","Segment10",1}},
      {"Segment12",{"tdcFront","Segment11",1}},
      {"Segment13",{"tdcMain","Segment12",1}},
      {"Segment14",{"tdc","Segment13",1}},
      {"Segment15",{"tdc","Segment14",1}},
      {"Segment16",{"tdc","Segment15",1}},
      {"Segment17",{"tdc","Segment16",1}},
      {"Segment18",{"tdc","Segment17",1}},
      {"Segment19",{"tdc","Segment18",1}},
      {"Segment20",{"tdc","Segment19",1}},
      {"Segment21",{"tdc","Segment20",1}},
      {"Segment22",{"tdcShort","Segment21",1}},
      {"Segment23",{"tdc","Segment22",1}},
      {"Segment24",{"tdc","Segment23",1}},
      {"Segment25",{"tdc","Segment24",1}},
      {"Segment26",{"spfLong","Segment25",1}},
      {"Segment27",{"spfLong","Segment26",2}},
      {"Segment28",{"spfLong","Segment27",2}},
      {"Segment29",{"spfLong","Segment28",2}},
      {"Segment30",{"tdcMain","Segment12",1}},
      {"Segment31",{"spfAngle","Segment30",1}},
      {"Segment32",{"spfAngle","Segment31",1}},
      {"Segment33",{"spfAngle","Segment32",1}},
      {"Segment34",{"spf","Segment33",1}},
      {"Segment35",{"spf","Segment34",1}},
      {"Segment36",{"spf","Segment35",1}},
      {"Segment37",{"spf","Segment36",1}},
      {"Segment38",{"spf","Segment37",1}},
      {"Segment39",{"spf","Segment38",1}},
      {"Segment40",{"spfFar","Segment39",1}},
      {"Segment41",{"spfFar","Segment40",1}},
      {"Segment42",{"spfFar","Segment41",1}},
      {"Segment43",{"spfFar","Segment42",1}},
      {"Segment44",{"spfFar","Segment43",1}},
      {"Segment45",{"spfFar","Segment44",3}},
      {"Segment46",{"spfFar","Segment44",2}}, // 44 is correct
      {"Segment47",{"spfFar","Segment46",1}},
      {"Segment48",{"spfFar","Segment47",1}},
      {"Segment49",{"spfFar","Segment48",1}}
    });
  const int voidCell(74123);


  // build injection hall first:
  injectionHall->addInsertCell(voidCell);
  injectionHall->createAll(System,FCOrigin,sideIndex);


  // special case of Segment10 : Segment26/27/28/29
  std::shared_ptr<attachSystem::BlockZone> seg10Zone;

  for(const std::string& BL : buildOrder)
    {
      if (activeINJ.find(BL)!=activeINJ.end())
	{
	  SegTYPE::const_iterator mc=SegMap.find(BL);
	  if (mc==SegMap.end())
	    throw ColErr::InContainerError<std::string>(BL,"Beamline");
	  const LinkTYPE seglink=segmentLinkMap.at(BL);

	  const std::string& bzName=std::get<0>(seglink);
	  const std::string& prevName=std::get<1>(seglink);
	  const size_t& prevIndex=std::get<2>(seglink);

	  SegTYPE::const_iterator prevC=SegMap.find(prevName);
	  const TDCsegment* prevSegPtr=
	    (prevC!=SegMap.end()) ?  prevC->second.get() : nullptr;
	  const std::shared_ptr<TDCsegment>& segPtr=mc->second;

	  std::shared_ptr<attachSystem::BlockZone> buildZone=
	    buildInnerZone(System,BL,bzName);

	  segPtr->setInnerZone(buildZone.get());
	  segPtr->registerPrevSeg(prevSegPtr,prevIndex);

	  std::vector<std::string> sideSegNames;
	  if (BL=="Segment10")
	    {
	      std::shared_ptr<attachSystem::BlockZone> secondZone=
		buildInnerZone(System,"Segment10B","tdcFront");
	      segPtr->setNextZone(secondZone.get());
	    }
	  if (BL=="Segment30")
	    sideSegNames={"Segment13","Segment14","Segment15"};

	  if (BL=="Segment46")
	    sideSegNames={"Segment44","Segment45"};

	  if (BL=="Segment47")
	    sideSegNames={"Segment45","Segment46"};

	  if (BL=="Segment48")
	    sideSegNames={"Segment47"};

	  if (BL=="Segment49") // same treatment as Segment10
	    {
	      std::shared_ptr<attachSystem::BlockZone> secondZone=
		buildInnerZone(System,"Segment49B","spfBehindBackWall");
	      segPtr->setNextZone(secondZone.get());
	    }

	  // Add side segments:
	  for(const std::string& sideItem : sideSegNames)
	    {
	      const TDCsegment* sidePtr=
		SegMap.find(sideItem)->second.get();
	      if (sidePtr->isBuilt())
		segPtr->registerSideSegment(sidePtr);
	    }

	  if (BL=="Segment48")   // SPECIAL CHANGE OF FRONT
	    {
	      SegTYPE::const_iterator ci=SegMap.find("Segment47");
	      if (ci!=SegMap.end())
		{
		  const TDCsegment* seg47Ptr=ci->second.get();
		  buildZone->setFront(seg47Ptr->getFullRule(2));
		}
	    }

	  segPtr->setInnerZone(buildZone.get());
	  segPtr->removeSpaceFillers(System);

	  segPtr->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));
	  segPtr->insertPrevSegment(System,prevSegPtr);

	  segPtr->createBeamLink(System.getDataBase());
	  if (!noCheck)
	    segPtr->totalPathCheck(System.getDataBase(),0.1);
	  if (pointCheck)
	    segPtr->writePoints();

	  if (BL=="Segment47")   // SPECIAL REMOVAL
	    {
	      SegTYPE::const_iterator ci=SegMap.find("Segment45");
	      if (ci!=SegMap.end())
		{
		  const TDCsegment* seg45Ptr=ci->second.get();
		  if (seg45Ptr->hasActiveCells())
		    {
		      const int SN(seg45Ptr->getLinkSurf(2));
		      const int SNback(segPtr->getLinkSurf(2));
		      const int CN(buildZone->getInsertCells()[0]);
		      MonteCarlo::Object* OPtr=System.findObject(CN);
		      if (!OPtr)
			throw ColErr::InContainerError<int>
			  (CN,"Object not found for Seg47");
		      OPtr->substituteSurf(SN,SNback,0);
		    }
		}
	    }
	}
    }

  reconstructInjectionHall(System);



  return;
}


}   // NAMESPACE tdcSystem
