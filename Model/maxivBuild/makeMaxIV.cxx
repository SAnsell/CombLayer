/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/makeMaxIV.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNMaxIV FOR A PARTICULAR PURPOSE.  See the
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
#include "support.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "LinkSupport.h"

#include "R1Ring.h"
#include "BALDER.h"
#include "COSAXS.h"
#include "MAXPEEM.h"

#include "makeMaxIV.h"

namespace xraySystem
{

makeMaxIV::makeMaxIV() :
   r1Ring(new R1Ring("R1Ring"))
 /*!
    Constructor
 */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(r1Ring);
}


makeMaxIV::~makeMaxIV()
  /*!
    Destructor
  */
{}

void
makeMaxIV::buildR1Ring(Simulation& System,
		       const mainSystem::inputParam& IParam)
  /*!
    Build the R1-ring based on segment needed
    \param System :: Simulation 
    \param IParam :: Input paramters
  */
{
  ELog::RegMethod RegA("makeMaxIV","buildR1Ring");

  const int voidCell(74123);


  r1Ring->addInsertCell(voidCell);
  r1Ring->createAll(System,World::masterOrigin(),0);

  const size_t NSet=IParam.setCnt("beamlines");
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",j);
      size_t index=0;
      while(index<NItems)  // min of one name
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index);

	  if (BL=="MAXPEEM")  // sector 11
	    {
	      MAXPEEM BL("MaxPeem");
	      BL.setRing(r1Ring);
	      BL.build(System,*r1Ring,
		       r1Ring->getSideIndex("OpticCentre7"));
	    }
	  index++;
	}
    }
  return;
}  
  
bool
makeMaxIV::makeBeamLine(Simulation& System,
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
  ELog::RegMethod RegA("makeMaxIV","makeBeamLine");

  static const std::set<std::string> beamNAMES
    ({"BALDER","COSAXS"});

  bool outFlag(0);  

  typedef std::map<std::string,std::vector<std::string>> mTYPE;
  mTYPE stopUnits=IParam.getMapItems("stopPoint");

  // create a map of beamname : stopPoint [or All : stoppoint]
  std::string stopPoint;
  std::map<std::string,std::string> beamStop;
  for(const mTYPE::value_type& SP : stopUnits)
    {
      if (beamNAMES.find(SP.first)==beamNAMES.end())
	stopPoint=SP.first;
      else if (!SP.second.empty())
	beamStop.emplace(SP.first,SP.second.front());
    }
    
  const size_t NSet=IParam.setCnt("beamlines");
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",j);
      size_t index=0;
      while(index<NItems)  // min of one name
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index);
	  const std::string FCName=
	    IParam.getValue<std::string>("beamlines",j,index+1);
	  const long int linkIndex=
	    IParam.getValue<long int>("beamlines",j,index+2);
	  index+=3;

          const attachSystem::FixedComp* FCOrigin=
	    System.getObjectThrow<attachSystem::FixedComp>
	    (FCName,"FixedComp not found for origin");

	  std::map<std::string,std::string>::const_iterator mc;
	  mc=beamStop.find(BL);
	  const std::string activeStop=
	    (mc!=beamStop.end()) ? mc->second : stopPoint;
	  if (BL=="BALDER")
	    {
	      BALDER BL("Balder");
	      if (!activeStop.empty())
		{
		  ELog::EM<<"Stop Point:"<<activeStop<<ELog::endDiag;
		  BL.setStopPoint(activeStop);
		}
	      BL.build(System,*FCOrigin,linkIndex);
	      outFlag=1;
	    }
	  else if (BL=="COSAXS")
	    {
	      COSAXS BL("Cosaxs");
	      BL.build(System,*FCOrigin,linkIndex);
	      outFlag=1;
	    }
	}
    }

  return outFlag;
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
  int voidCell(74123);

  if (makeBeamLine(System,IParam))  // 3GeV Ring
    ELog::EM<<"=Finished 3.0GeV Ring="<<ELog::endDiag;
  else
    {
      buildR1Ring(System,IParam);
      ELog::EM<<"Finished 1.5GeV Ring"<<ELog::endDiag;
    }

  return;
}


}   // NAMESPACE xraySystem

