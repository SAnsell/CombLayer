/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: linac/TDC.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "InnerZone.h"
#include "ModelSupport.h"
#include "generateSurf.h"

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
#include "Segment30.h"
#include "Segment31.h"
#include "Segment32.h"

#include "TDC.h"

namespace tdcSystem
{

TDC::TDC(const std::string& KN) :
  attachSystem::FixedOffset(KN,6),
  attachSystem::CellMap(),
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
    { "Segment30",std::make_shared<Segment30>("SPF30") },
    { "Segment31",std::make_shared<Segment31>("SPF31") },
    { "Segment32",std::make_shared<Segment32>("SPF32") }

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

  static int BI(buildIndex);   // keep regions in order and unique

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
  return HeadRule(Out);

}

std::unique_ptr<attachSystem::InnerZone>
TDC::buildInnerZone(const FuncDataBase& Control,
		    const std::string& regionName)
  /*!
    Set the regional buildzone
    \param Control :: FuncData base for building if needed
    \param regionName :: Zone name
  */
{
  ELog::RegMethod RegA("TDC","buidInnerZone");

  // FrontSurf : BackSurf : Cell : Cell(if not empty)
  typedef std::tuple<std::string,std::string,std::string,std::string> RTYPE;
  typedef std::map<std::string,RTYPE> RMAP;

  // front : back : Insert
  const static RMAP regZones
    ({
      {"l2spf",{"Front","#MidWall","LinearVoid",""}},
      {"l2spfTurn",{"KlystronWall","#MidWall","LinearVoid",""}},
      {"l2spfAngle",{"KlystronWall","#MidAngleWall","LinearVoid",""}},
      {"tdcFront"  ,{"TDCCorner","#TDCMid","SPFVoid","TVoid"}},
      {"tdc"  ,{"TDCCorner","#TDCMid","SPFVoid","LongVoid"}},
      {"spf"  ,{"TDCMid","#Back","LongVoid",""}}
    });

  RMAP::const_iterator rc=regZones.find(regionName);
  if (rc==regZones.end())
    throw ColErr::InContainerError<std::string>(regionName,"regionZones");


  const RTYPE& walls=rc->second;
  const std::string& frontSurfName=std::get<0>(walls);
  const std::string& backSurfName=std::get<1>(walls);
  const std::string& voidName=std::get<2>(walls);
  const std::string& voidNameB=std::get<3>(walls);

  std::unique_ptr<attachSystem::InnerZone> buildZone=
    std::make_unique<attachSystem::InnerZone>(*this,cellIndex);

  buildZone->setFront(injectionHall->getSurfRules(frontSurfName));
  buildZone->setBack(injectionHall->getSurfRule(backSurfName));
  buildZone->setSurround
    (buildSurround(Control,regionName,"Origin"));
  buildZone->setInsertCells(injectionHall->getCells(voidName));
  if (!voidNameB.empty())
    buildZone->addInsertCells(injectionHall->getCells(voidNameB));

  return buildZone;
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
      "Segment32"
    });

  typedef std::tuple<std::string,std::string> LinkTYPE;
  static const std::map<std::string,LinkTYPE> segmentLinkMap
    ({
      {"Segment1",{"l2spf",""}},
      {"Segment2",{"l2spf","Segment1"}},
      {"Segment3",{"l2spf","Segment2"}},
      {"Segment4",{"l2spf","Segment3"}},
      {"Segment5",{"l2spfTurn","Segment4"}},
      {"Segment6",{"l2spfTurn","Segment5"}},
      {"Segment7",{"l2spfAngle","Segment6"}},
      {"Segment8",{"l2spfAngle","Segment7"}},
      {"Segment9",{"l2spfAngle","Segment8"}},
      {"Segment10",{"l2spfAngle","Segment9"}},
      {"Segment11",{"tdcFront","Segment10"}},
      {"Segment12",{"tdcFront","Segment11"}},
      {"Segment13",{"tdcFront","Segment12"}},
      {"Segment14",{"tdc",""}},
      {"Segment15",{"tdc","Segment14"}},
      {"Segment16",{"tdc","Segment15"}},
      {"Segment17",{"tdc","Segment16"}},
      {"Segment18",{"tdc","Segment17"}},
      {"Segment19",{"tdc","Segment18"}},
      {"Segment20",{"tdc","Segment19"}},
      {"Segment21",{"tdc","Segment20"}},
      {"Segment22",{"tdc","Segment21"}},
      {"Segment23",{"tdc","Segment22"}},
      {"Segment24",{"tdc","Segment23"}},
      {"Segment25",{"spf","segment24"}},
      {"Segment30",{"tdc","Segment29"}},
      {"Segment31",{"tdc","Segment30"}},
      {"Segment32",{"tdc","Segment32"}}

    });
  const int voidCell(74123);


  // build injection hall first:
  injectionHall->addInsertCell(voidCell);
  injectionHall->createAll(System,FCOrigin,sideIndex);

  // special case of Segment10 :

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
	  SegTYPE::const_iterator prevC=SegMap.find(prevName);

	  const std::shared_ptr<TDCsegment>& segPtr=mc->second;

	  std::unique_ptr<attachSystem::InnerZone> buildZone=
	    buildInnerZone(System.getDataBase(),bzName);
	  std::unique_ptr<attachSystem::InnerZone> secondZone;
	  if (prevC!=SegMap.end())
	    {
	      const std::shared_ptr<TDCsegment>& prevPtr(prevC->second);
	      if (prevPtr->hasLastSurf())
		{
		  buildZone->setFront(prevPtr->getLastSurf());
		  segPtr->setFrontSurf(prevPtr->getLastSurf());
		}
	    }

	  buildZone->constructMasterCell(System);
	  segPtr->setInnerZone(buildZone.get());
	  // special case of Segment10 :

	  if (BL=="Segment10")
	    {
	      secondZone=buildInnerZone(System.getDataBase(),"tdcFront");
	      segPtr->setNextZone(secondZone.get());
	    }

	  segPtr->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));

	  segPtr->totalPathCheck(System.getDataBase(),0.1);
	}
    }
  return;
}


}   // NAMESPACE tdcSystem
