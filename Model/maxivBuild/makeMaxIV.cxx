/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   maxivBuild/makeMaxIV.cxx
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
#include <utility>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <array>
#include <algorithm>
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"
#include "BlockZone.h"
#include "World.h"


#include "InjectionHall.h"
#include "R1Ring.h"
#include "R1Beamline.h"
#include "R3Ring.h"
#include "R3Beamline.h"
#include "BALDER.h"
#include "COSAXS.h"
#include "SOFTIMAX.h"
#include "DANMAX.h"
#include "MAXPEEM.h"
#include "FLEXPES.h"
#include "FORMAX.h"
#include "MICROMAX.h"
#include "SPECIES.h"

#include "TDC.h"

#include "makeMaxIV.h"

namespace xraySystem
{

makeMaxIV::makeMaxIV() :
  r1Ring(new R1Ring("R1Ring")),
  r3Ring(new R3Ring("R3Ring")),
  tdc(new tdcSystem::TDC("TDC"))

 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(r1Ring);
  OR.addObject(r3Ring);
  OR.addObject(tdc);
}


makeMaxIV::~makeMaxIV()
  /*!
    Destructor
  */
{}


std::string
makeMaxIV::getActiveStop(const std::map<std::string,std::string>& beamStop,
			 const std::string& BL) const
  /*!
    Get the current active stoppoint based on the beamline
    \param beamStop :: Active units from IParam
    \param BL :: Current beamline
    \return active stop unit
  */
{
  ELog::RegMethod RegA("makeMaxIV","getActiveStop");

  std::map<std::string,std::string>::const_iterator mc;
  mc=beamStop.find(BL);

  return (mc!=beamStop.end()) ? mc->second : "";
}


void
makeMaxIV::populateStopPoint(const mainSystem::inputParam& IParam,
			     const std::set<std::string>& beamNAMES,
			     std::map<std::string,std::string>& beamStop) const
  /*!
    Build the beamstop map of stop points
    \param IParam ::Input parameter
    \param beamNAMES ::  beamlines to consider
    \param beamStop ::  beamline : Stop point
  */
{
  ELog::RegMethod RegA("makeMaxIV","populateStopPoint");

  typedef std::map<std::string,std::vector<std::string>> mTYPE;
  mTYPE stopUnits=IParam.getMapItems("stopPoint");

  // create a map of beamname : stopPoint [or All : stoppoint]
  std::string stopPoint;

  for(const mTYPE::value_type& SP : stopUnits)
    {
      if (beamNAMES.find(SP.first)==beamNAMES.end())
	{
	  // make generic
	  for(const std::string Item : beamNAMES)
	    beamStop.emplace(Item,SP.first);    // doesn't overwrite specific
	}
      else if (!SP.second.empty())
	beamStop[SP.first]=SP.second.front();   // ensures overwriting
    }
  return;
}

bool
makeMaxIV::buildInjection(Simulation& System,
			  const mainSystem::inputParam& IParam)
  /*!
    Build the SPF/linac hall
    \param System :: Simulation
    \param IParam :: Input paramters
  */
{
  ELog::RegMethod RegA("makeMaxIV","buildInjection");

  // names : {set of real components to build}
  typedef const std::map<std::string,std::set<std::string> > ITYPE;
  static ITYPE injectNAMES

    ({ {"All",
	{"Segment1","Segment2","Segment3",
	 "Segment4", "Segment5", "Segment6",
	 "Segment7", "Segment8", "Segment9",
	 "Segment10", "Segment11", "Segment12",
	 "Segment13", "Segment14", "Segment15",
	 "Segment16", "Segment17", "Segment18",
	 "Segment19", "Segment20", "Segment21",
	 "Segment22", "Segment23", "Segment24",
	 "Segment25", "Segment26", "Segment27",
	 "Segment28", "Segment29", "Segment30",
	 "Segment31", "Segment32", "Segment33",
	 "Segment34", "Segment35", "Segment36",
	 "Segment37", "Segment38", "Segment39",
	 "Segment40", "Segment41", "Segment42",
	 "Segment43", "Segment44", "Segment45",
	 "Segment46", "Segment47", "Segment48",
	 "Segment49"

	} },
       {"L2SPF",
	{"Segment1","Segment2","Segment3",
	 "Segment4", "Segment5", "Segment6",
	 "Segment7", "Segment8", "Segment9",
	 "Segment10", "Segment11", "Segment12",
	 "Segment13"
	} },
       {"TDC",{"Segment14","Segment15","Segment16",
	       "Segment17","Segment18","Segment19",
	       "Segment20","Segment21","Segment22",

	       "Segment23","Segment24","Segment25",
	       "Segment27","Segment28","Segment29",
	       "Segment30",
	       "Segment31","Segment32","Segment33",
	       "Segment34"}},
       {"SPF",{"Segment30","Segment31","Segment32",
	       "Segment33","Segment34","Segment35",
	       "Segment36","Segment37","Segment38",
	       "Segment39","Segment40","Segment41",
	       "Segment42","Segment43","Segment44",
	       "Segment45","Segment46","Segment47",
	       "Segment48","Segment49"}},

       {"Segment1",{} },                             // first only
       {"Segment2",{} },                             // second only
       {"Segment3",{} },                             // third only
       {"Segment4",{} },                             // forth only
       {"Segment5",{} },                             // fifth only
       {"Segment6",{} },                             // 6th only
       {"Segment7",{} },                             // 7h only
       {"Segment8",{} },                             // 8th only
       {"Segment9",{} },                             // 9th only
       {"Segment10",{} },                            // wall unit
       {"Segment11",{} },                            // TDC
       {"Segment12",{} },                            // TDC
       {"Segment13",{} },                            // TDC
       {"Segment14",{} },                            // 14th only
       {"Segment15",{} },                            // 15th only
       {"Segment16",{} },                            // 16th only
       {"Segment17",{} },                            // 17th only
       {"Segment18",{} },                            // 18th only
       {"Segment19",{} },                            // 19th only
       {"Segment20",{} },                            // 20th only
       {"Segment21",{} },                            // 21st only
       {"Segment22",{} },                            // 22nd only
       {"Segment23",{} },                            // 23rd only
       {"Segment24",{} },                            // 24th only
       {"Segment25",{} },                            // 25th only
       {"Segment26",{} },                            // 26th only
       {"Segment27",{} },                            // 27th only
       {"Segment28",{} },                            // 28th only
       {"Segment29",{} },                            // 29th only
       {"Segment30",{} },                            // 30th only
       {"Segment31",{} },                            // 31st only
       {"Segment32",{} },                            // 32nd only
       {"Segment33",{} },                            // 33rd only
       {"Segment34",{} },                            // 34th only
       {"Segment35",{} },                            // 35th only
       {"Segment36",{} },                            // 36th only
       {"Segment37",{} },                            // 37th only
       {"Segment38",{} },                            // 38th only
       {"Segment39",{} },                            // 39th only
       {"Segment40",{} },                            // 40th only
       {"Segment41",{} },                            // 41st only
       {"Segment42",{} },                            // 42nd only
       {"Segment43",{} },                            // 43st only
       {"Segment44",{} },                            // 44th only
       {"Segment45",{} },                            // 45th only
       {"Segment46",{} },                            // 46th only
       {"Segment47",{} },                            // 47th only
       {"Segment48",{} },                            // 48th only
       {"Segment49",{} }                             // 49th only

    });

  bool activeLinac(0);
  const size_t NSet=IParam.setCnt("beamlines");  // converted from
                                                 //  defaultConfig linac
  std::set<std::string> activeINJ;
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",j);
      size_t index=0;
      while(index<NItems)  // min of one name
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index);
	  if (BL=="LINAC" || BL=="SPF")  // default build
	    activeLinac=1;
	  else
	    {
	      ITYPE::const_iterator mc=injectNAMES.find(BL);
	      if (mc != injectNAMES.end())
		{
		  if (mc->second.empty())
		    {
		      activeLinac=1;
		      activeINJ.emplace(BL);
		    }
		  else
		    {
		      for(const std::string& item : mc->second)
			activeINJ.emplace(item);
		    }
		}
	    }
	  index++;
	}
    }
  if (!activeLinac) return 0;


  if (IParam.flag("noLengthCheck"))
    tdc->setNoLengthCheck();
  if (IParam.flag("pointCheck"))
    tdc->setPointCheck();
  tdc->setActive(activeINJ);
  tdc->createAll(System,World::masterOrigin(),0);

  return 1;
}

bool
makeMaxIV::buildR1Ring(Simulation& System,
		       const mainSystem::inputParam& IParam)
  /*!
    Build the R1-ring based on segment needed
    \param System :: Simulation
    \param IParam :: Input paramters
  */
{
  ELog::RegMethod RegA("makeMaxIV","buildR1Ring");

  static const std::map<std::string,std::string> beamNAMES
    ({ {"FLEXPES","OpticCentre6"},
	{"MAXPEEM","OpticCentre7"},
	{"SPECIES","OpticCentre8"}
    });


  // Determine if R1Ring/beamlines need to be built
  std::set<std::string> activeBL;
  bool activeR1(0);
  const size_t NSet=IParam.setCnt("beamlines");

  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",j);
      size_t index=0;
      while(index<NItems)  // min of one name
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index);

	  if (BL=="R1RING" || BL=="RING1")
	    activeR1=1;
	  else if (beamNAMES.find(BL) != beamNAMES.end())
	    {
	      activeR1=1;
	      activeBL.insert(BL);
	    }
	  index++;
	}
    }

  if (!activeR1) return 0;

  const int voidCell(74123);
  r1Ring->addInsertCell(voidCell);
  r1Ring->createAll(System,World::masterOrigin(),0);

  std::map<std::string,std::string> beamStop;
  populateStopPoint(IParam,activeBL,beamStop);


  for(const std::string& BL : activeBL)
    {
      // StopPoint
      const std::string activeStop=getActiveStop(beamStop,BL);
      //\todo make beamlines inherrit from generic
      std::unique_ptr<R1Beamline> BLPtr;
      if (BL=="FLEXPES")  // sector
	BLPtr.reset(new FLEXPES("FlexPes"));
      else if (BL=="MAXPEEM")
	BLPtr.reset(new MAXPEEM("MaxPeem"));
      else if (BL=="SPECIES")
	BLPtr.reset(new SPECIES("Species"));

      if (!activeStop.empty())
	{
	  BLPtr->setStopPoint(activeStop);
	}

      BLPtr->setRing(r1Ring);
      BLPtr->build(System,*r1Ring,
		   r1Ring->getSideIndex(beamNAMES.at(BL)));
    }

  return 1;    // R1 Built
}

bool
makeMaxIV::buildR3Ring(Simulation& System,
		      const mainSystem::inputParam& IParam)
/*!
    Build a beamline based on LineType
     -- to construct a beamline the name of the guide Item
     and the beamline typename is required
    \param System :: Simulation
    \param IParam :: Input paramters
    \retrun true if object(s) built
  */
{
  ELog::RegMethod RegA("makeMaxIV","buildR3Ring");

  static const std::map<std::string,std::string> beamNAMES
    ({ {"BALDER","OpticCentre1"},
       {"COSAXS","OpticCentre1"},
       {"SOFTIMAX","OpticCentre1"},
       {"DANMAX","OpticCentre1"},
       {"FORMAX","OpticCentre1"},  // was 8
       {"MICROMAX","OpticCentre1"}
    });

  // Determine if R1Ring/beamlines need to be built
  std::set<std::string> activeBL;
  bool activeR3(0);
  const size_t NSet=IParam.setCnt("beamlines");

  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",j);
      size_t index=0;
      while(index<NItems)  // min of one name
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index);

	  if (BL=="R3RING")
	    activeR3=1;
	  else if (beamNAMES.find(BL) != beamNAMES.end())
	    {
	      activeR3=1;
	      activeBL.insert(BL);
	    }
	  index++;
	}
    }
  if (!activeR3) return 0;


  const int voidCell(74123);
  r3Ring->addInsertCell(voidCell);
  r3Ring->createAll(System,World::masterOrigin(),0);

  std::map<std::string,std::string> beamStop;
  populateStopPoint(IParam,activeBL,beamStop);


  for(const std::string& BL : activeBL)
    {
      // StopPoint
      const std::string activeStop=getActiveStop(beamStop,BL);
      //\todo make beamlines inherrit from generic
      std::unique_ptr<R3Beamline> BLPtr;
      if (BL=="BALDER")  // sector
	BLPtr.reset(new BALDER("Balder"));
      else if (BL=="COSAXS")
	BLPtr.reset(new COSAXS("Cosaxs"));
      else if (BL=="SOFTIMAX")
	BLPtr.reset(new SOFTIMAX("SoftiMAX"));
      else if (BL=="DANMAX")
	BLPtr.reset(new DANMAX("Danmax"));
      else if (BL=="FORMAX")
	BLPtr.reset(new FORMAX("Formax"));
      else if (BL=="MICROMAX")
	BLPtr.reset(new MICROMAX("MicroMax"));

      if (!activeStop.empty())
	{
	  ELog::EM<<"Stop Point:"<<activeStop<<ELog::endDiag;
	  BLPtr->setStopPoint(activeStop);
	}
      BLPtr->setRing(r3Ring);
      BLPtr->build(System,*r3Ring,
		   r3Ring->getSideIndex(beamNAMES.at(BL)));
    }
  return 1;    // R3 Built
}

void
makeMaxIV::build(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeMaxIV","build");

  //  const FuncDataBase& Control=System.getDataBase();

  if (buildR3Ring(System,IParam))  // 3GeV Ring
    ELog::EM<<"=Finished 3.0GeV Ring="<<ELog::endDiag;

  else if(buildR1Ring(System,IParam))
    ELog::EM<<"Finished 1.5GeV Ring"<<ELog::endDiag;

  else if (buildInjection(System,IParam))  // Injection Hall
    ELog::EM<<"=Finished Linac/SPF System="<<ELog::endDiag;

  else
    ELog::EM<<"NO Linac/ SPF / R1 / R3 Ring built"<<ELog::endCrit;

  return;
}


}   // NAMESPACE xraySystem
