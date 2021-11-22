/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/flukaSourceSelector.cxx
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "inputParam.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "inputSupport.h"
#include "SourceCreate.h"
#include "SourceBase.h"
#include "World.h"
#include "sourceDataBase.h"
#include "flukaSourceSelector.h"

namespace SDef
{
  
void 
flukaSourceSelection(Simulation& System,
		     const mainSystem::inputParam& IParam)
  /*!
    Build the source based on the input parameter table
    - sdefObj Object linkPt Distance [along y (or vector)]
    \param System :: Simulation to use
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("flukaSourceSelector[F]","flukaSourceSelection");
  
  const mainSystem::MITYPE inputMap=IParam.getMapItems("sdefMod");

  attachSystem::FixedUnit beamAxis("beamAxis");
  const bool axisFlag(IParam.flag("sdefVec"));
  if (axisFlag)
    {
      size_t itemCnt(0);
      const Geometry::Vec3D Org=
	mainSystem::getDefNamedPoint(System,IParam,"sdefVec",
				     0,itemCnt,Geometry::Vec3D(0,0,0));
      
      const Geometry::Vec3D Axis=
	mainSystem::getDefNamedAxis(System,IParam,"sdefVec",
				     1,itemCnt,Geometry::Vec3D(0,1,0));
      const Geometry::Vec3D ZAxis=
	mainSystem::getDefNamedAxis(System,IParam,"sdefVec",
				    2,itemCnt,Geometry::Vec3D(0,0,1));
      beamAxis.createUnitVector(Org,Axis,ZAxis);
    }
  
  const std::string DObj=IParam.getDefValue<std::string>("","sdefObj",0);
  const std::string DSnd=IParam.getDefValue<std::string>("","sdefObj",1);
  const std::string Dist=IParam.getDefValue<std::string>("","sdefObj",2);

  double D;
  Geometry::Vec3D DOffsetStep(0,0,0);
  if (!StrFunc::convert(Dist,DOffsetStep) && 
      StrFunc::convert(Dist,D))
    DOffsetStep[1]=D;
  
  const attachSystem::FixedComp& FC= (DObj.empty()) ?
    ((axisFlag) ? beamAxis : World::masterOrigin()) :
    *(System.getObjectThrow<attachSystem::FixedComp>(DObj,"Object not found"));

  const long int linkIndex=(DSnd.empty()) ? 0 :  FC.getSideIndex(DSnd);

  // NOTE: No return to allow active SSW systems
  const size_t NSDef(IParam.setCnt("sdefType"));

  std::string sName;
  std::string eName;
  for(size_t sdefIndex=0;sdefIndex<NSDef;sdefIndex++)
    {
      const std::string sdefType=IParam.getValue<std::string>
	("sdefType",sdefIndex,0);

      ELog::EM<<"SDEF TYPE ["<<sdefIndex<<"] == "<<sdefType<<ELog::endDiag;
      
      if (sdefType=="Wiggler")                       // blader wiggler
	sName=SDef::createWigglerSource(inputMap,FC,linkIndex);
      
      else if (sdefType=="Beam" || sdefType=="beam")
	sName=SDef::createBeamSource(inputMap,"beamSource",FC,linkIndex);

      
      else if (sdefType=="external" || sdefType=="External" ||
	       sdefType=="source" || sdefType=="Source")
	eName=SDef::createFlukaSource(inputMap,"flukaSource",FC,linkIndex);
	 
      else
	{
	  ELog::EM<<"sdefType :\n"
	    "Beam :: Test Beam [Radial] source \n"
	    "Wiggler :: Wiggler Source for balder \n"
	    "External/Source :: External source from source.f \n"
		  <<ELog::endBasic;
	}
    }
  
  ELog::EM<<"Source name(s) == "<<sName<<" "<<eName<<ELog::endDiag;
  processPolarization(inputMap,sName);
  
  if (!IParam.flag("sdefVoid") && !sName.empty())
    System.setSourceName(sName);
  if (!eName.empty())
    {
      if (sName.empty())
	throw ColErr::EmptyValue<std::string>
	  ("sourceName empty and extraName valid:"+eName);
      
      System.setExtraSourceName(eName);
    }
  
  return;
}

void
processPolarization(const mainSystem::MITYPE& inputMap,
		    const std::string& sourceName)
  /*!
    Process the polarization vector
    \param inputMap :: IParam input stream
  */
{
  ELog::RegMethod RegA("SourceSelector[F]","processPolarization");

  sourceDataBase& SDB=sourceDataBase::Instance();

  SDef::SourceBase* SPtr=SDB.getSource<SDef::SourceBase>(sourceName);
  if (SPtr)
    {
      mainSystem::MITYPE::const_iterator mc=inputMap.find("polarization");
      if (mc!=inputMap.end())
	{
	  ELog::EM<<"POLAR"<<ELog::endDiag;
	  Geometry::Vec3D PVec;

	  const std::vector<std::string>& IVec=mc->second;
	  if (!IVec.empty() && StrFunc::convert(IVec.front(),PVec))
	    {
	      double Pfrac(1.0);
	      if (IVec.size()>1)
		StrFunc::convert(IVec[1],Pfrac);

	      SPtr->setPolarization(PVec,Pfrac);		  
	    }
	}
    }
  return;
}
  

  
} // NAMESPACE SDef
