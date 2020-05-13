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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "FixedOffsetUnit.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "InnerZone.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "InjectionHall.h"
#include "TDCsegment.h"
#include "L2SPFsegment1.h"
#include "L2SPFsegment2.h"

#include "L2SPFsegment14.h"
#include "L2SPFsegment15.h"

#include "TDC.h"

namespace tdcSystem
{

TDC::TDC(const std::string& KN) :
  attachSystem::FixedOffset(KN,6),
  attachSystem::CellMap(),
  injectionHall(new InjectionHall("InjectionHall")),
  l2spf1(new L2SPFsegment1("L2SPF1")),
  l2spf2(new L2SPFsegment2("L2SPF2")),
  l2spf14(new L2SPFsegment14("L2SPF14")),
  l2spf15(new L2SPFsegment15("L2SPF15"))
  /*!
    Constructor
    \param KN :: Keyname
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(injectionHall);
  OR.addObject(l2spf1);
  OR.addObject(l2spf2);
  OR.addObject(l2spf14);
  OR.addObject(l2spf15);

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
  
  const double outerLeft=
    Control.EvalTail<double>(keyName+regionName,keyName,"OuterLeft");
  const double outerRight=
    Control.EvalTail<double>(keyName+regionName,keyName,"OuterRight");
  const double outerTop=
    Control.EvalTail<double>(keyName+regionName,keyName,"OuterTop");
  const double outerFloor=
    Control.EvalDefTail<double>(keyName+regionName,
				keyName,"OuterFloor",-5e10); // large-neg

  const attachSystem::FixedOffsetUnit injFC
    (Control,keyName+regionName,*injectionHall,injectionPt);

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
  ELog::RegMethod RegA("TDC","getBuildZone");

  typedef std::tuple<std::string,std::string,std::string> RTYPE;
  typedef std::map<std::string,RTYPE> RMAP;

  // front : back : Insert
  const static RMAP regZones
    ({
      {"l2spf",{"Front","#MidWall","LinearVoid"}},
      {"tdc"  ,{"TDCCorner","#TDCMid","SPFVoid"}}  
    });

  RMAP::const_iterator rc=regZones.find(regionName);
  if (rc==regZones.end())
    throw ColErr::InContainerError<std::string>(regionName,"regionZones");
  

  const RTYPE& walls=rc->second;
  const std::string& frontSurfName=std::get<0>(walls);
  const std::string& backSurfName=std::get<1>(walls);
  const std::string& voidName=std::get<2>(walls);
  
  std::unique_ptr<attachSystem::InnerZone> buildZone=
    std::make_unique<attachSystem::InnerZone>(*this,cellIndex);

  buildZone->setFront(injectionHall->getSurfRules(frontSurfName));
  buildZone->setBack(injectionHall->getSurfRule(backSurfName));
  buildZone->setSurround
    (buildSurround(Control,regionName,"Origin"));
  buildZone->setInsertCells(injectionHall->getCells(voidName));
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

  //  typedef std::tuple<std::string> ITYPE;
  static const std::map<std::string,std::string> segmentLinkMap
    ({
      {"L2SPFsegment1","l2spf"},
      {"L2SPFsegment2","l2spf"},
      {"L2SPFsegment14","tdc"},
      {"L2SPFsegment15","tdc"}
    });
  const int voidCell(74123);

  // build injection hall first:
  injectionHall->addInsertCell(voidCell);
  injectionHall->createAll(System,FCOrigin,sideIndex);

  for(const std::string& BL : activeINJ)
    {
      const std::string& bzName=segmentLinkMap.at(BL);
      std::unique_ptr<attachSystem::InnerZone> buildZone=
	buildInnerZone(System.getDataBase(),bzName);
      
      if (BL=="L2SPFsegment1")  
	{
	  l2spf1->setInnerZone(buildZone.get());
	  buildZone->constructMasterCell(System);
	  
	  l2spf1->addInsertCell(injectionHall->getCell("LinearVoid"));
	  l2spf1->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));
	}
      if (BL=="L2SPFsegment2")  
	{
	  if (l2spf1->hasLastSurf())
	    buildZone->setFront(l2spf1->getLastSurf());
	  buildZone->constructMasterCell(System);
	  
	  l2spf2->setInnerZone(buildZone.get());
	  l2spf2->addInsertCell(injectionHall->getCell("LinearVoid"));
	  l2spf2->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));
	}
      else if (BL=="L2SPFsegment14")
	{
	  //	  if (l2spf13->hasLastSurf())
	  //	    buildZone->setFront(l2spf13->getLastSurf());
	  l2spf14->setInnerZone(buildZone.get());
	  buildZone->constructMasterCell(System);
	  
	  l2spf14->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));
	}
      else if (BL=="L2SPFsegment15")
	{
	  if (l2spf14->hasLastSurf())
	    buildZone->setFront(l2spf14->getLastSurf());
	  l2spf15->setInnerZone(buildZone.get());
	  buildZone->constructMasterCell(System);
	  
	  l2spf15->createAll
	    (System,*injectionHall,injectionHall->getSideIndex("Origin"));
	}
    }

  return;
}


}   // NAMESPACE tdcSystem

