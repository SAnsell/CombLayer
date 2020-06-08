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
#include "L2SPFsegment1.h"
#include "L2SPFsegment2.h"
#include "L2SPFsegment3.h"
#include "L2SPFsegment4.h"
#include "L2SPFsegment5.h"
#include "L2SPFsegment6.h"
#include "L2SPFsegment7.h"
#include "L2SPFsegment8.h"
#include "L2SPFsegment9.h"
#include "L2SPFsegment10.h"
#include "L2SPFsegment11.h"
#include "L2SPFsegment12.h"
#include "L2SPFsegment13.h"

#include "TDCsegment14.h"
#include "TDCsegment15.h"
#include "TDCsegment16.h"
#include "TDCsegment17.h"
#include "TDCsegment18.h"
#include "TDCsegment19.h"
#include "TDCsegment20.h"

#include "TDC.h"

namespace tdcSystem
{

TDC::TDC(const std::string& KN) :
  attachSystem::FixedOffset(KN,6),
  attachSystem::CellMap(),
  injectionHall(new InjectionHall("InjectionHall")),
  SegMap
  ({
    { "L2SPFsegment1",std::make_shared<L2SPFsegment1>("L2SPF1") },
    { "L2SPFsegment2",std::make_shared<L2SPFsegment2>("L2SPF2") },
    { "L2SPFsegment3",std::make_shared<L2SPFsegment3>("L2SPF3") },
    { "L2SPFsegment4",std::make_shared<L2SPFsegment4>("L2SPF4") },
    { "L2SPFsegment5",std::make_shared<L2SPFsegment5>("L2SPF5") },
    { "L2SPFsegment6",std::make_shared<L2SPFsegment6>("L2SPF6") },
    { "L2SPFsegment7",std::make_shared<L2SPFsegment7>("L2SPF7") },
    { "L2SPFsegment8",std::make_shared<L2SPFsegment8>("L2SPF8") },
    { "L2SPFsegment9",std::make_shared<L2SPFsegment9>("L2SPF9") },
    { "L2SPFsegment10",std::make_shared<L2SPFsegment10>("L2SPF10") },
    { "L2SPFsegment11",std::make_shared<L2SPFsegment11>("L2SPF11") },
    { "L2SPFsegment12",std::make_shared<L2SPFsegment12>("L2SPF12") },
    { "TDCsegment14",std::make_shared<TDCsegment14>("TDC14") },
    { "TDCsegment15",std::make_shared<TDCsegment15>("TDC15") },
    { "TDCsegment16",std::make_shared<TDCsegment16>("TDC16") },
    { "TDCsegment17",std::make_shared<TDCsegment17>("TDC17") },
    { "TDCsegment18",std::make_shared<TDCsegment18>("TDC18") },
    { "TDCsegment19",std::make_shared<TDCsegment19>("TDC19") },
    { "TDCsegment20",std::make_shared<TDCsegment20>("TDC20") }
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
      {"tdc"  ,{"TDCCorner","#TDCMid","SPFVoid",""}}
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
      "L2SPFsegment1","L2SPFsegment2","L2SPFsegment3",
      "L2SPFsegment4","L2SPFsegment5","L2SPFsegment6",
      "L2SPFsegment7","L2SPFsegment8","L2SPFsegment9",
      "L2SPFsegment10","L2SPFsegment11","L2SPFsegment12",
      "L2SPFsegment13",
      "TDCsegment14","TDCsegment15","TDCsegment16",
      "TDCsegment17","TDCsegment18","TDCsegment19",
      "TDCsegment20","TDCsegment21","TDCsegment22"
    });
  
  typedef std::tuple<std::string,std::string> LinkTYPE;
  static const std::map<std::string,LinkTYPE> segmentLinkMap
    ({
      {"L2SPFsegment1",{"l2spf",""}},
      {"L2SPFsegment2",{"l2spf","L2SPFsegment1"}},
      {"L2SPFsegment3",{"l2spf","L2SPFsegment2"}},
      {"L2SPFsegment4",{"l2spf","L2SPFsegment3"}},
      {"L2SPFsegment5",{"l2spfTurn","L2SPFsegment4"}},
      {"L2SPFsegment6",{"l2spfTurn","L2SPFsegment5"}},
      {"L2SPFsegment7",{"l2spfAngle","L2SPFsegment6"}},
      {"L2SPFsegment8",{"l2spfAngle","L2SPFsegment7"}},
      {"L2SPFsegment9",{"l2spfAngle","L2SPFsegment8"}},
      {"L2SPFsegment10",{"l2spfAngle","L2SPFsegment9"}},
      {"L2SPFsegment11",{"tdcFront","L2SPFsegment10"}},
      {"L2SPFsegment12",{"tdcFront","L2SPFsegment11"}},
      {"TDCsegment14",{"tdc",""}},
      {"TDCsegment15",{"tdc","TDCsegment14"}},
      {"TDCsegment16",{"tdc","TDCsegment15"}},
      {"TDCsegment17",{"tdc","TDCsegment16"}},
      {"TDCsegment18",{"tdc","TDCsegment17"}},
      {"TDCsegment19",{"tdc","TDCsegment18"}},
      {"TDCsegment20",{"tdc","TDCsegment19"}}
    });
  const int voidCell(74123);


  // build injection hall first:
  injectionHall->addInsertCell(voidCell);
  injectionHall->createAll(System,FCOrigin,sideIndex);

  // special case of L2SPFsegment10 :
  
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
	  // special case of L2SPFsegment10 :
	  
	  if (BL=="L2SPFsegment10")
	    {
	      secondZone=buildInnerZone(System.getDataBase(),"tdcFront");
	      segPtr->setNextZone(secondZone.get());
	    }
	  
	  segPtr->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));
	  
	  // special case for join of wall
	  //      if (BL=="L2SPFsegment10")
	  //	processWallJoin();
	  
	  segPtr->totalPathCheck(System.getDataBase(),0.1);
	}
    }
  return;
}


}   // NAMESPACE tdcSystem
