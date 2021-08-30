/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/phitsSourceSelector.cxx
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
#include "phitsSourceSelector.h"

namespace SDef
{
  
void 
phitsSourceSelection(Simulation& System,
		     const mainSystem::inputParam& IParam)
  /*!
    Build the source based on the input parameter table
    - sdefObj Object linkPt Distance [along y (or vector)]
    \param System :: Simulation to use
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("phitsSourceSelector[F]","phitsSourceSelection");
  
  const mainSystem::MITYPE inputMap=IParam.getMapItems("sdefMod");

  attachSystem::FixedUnit beamAxis("beamAxis");
  const bool axisFlag(IParam.flag("sdefVec"));
  if (axisFlag)
    {
      
      const Geometry::Vec3D Org=
	IParam.getDefValue<Geometry::Vec3D>(Geometry::Vec3D(0,0,0),"sdefVec",0);
      const Geometry::Vec3D Axis=
	IParam.getDefValue<Geometry::Vec3D>(Geometry::Vec3D(0,1,0),"sdefVec",1);
      const Geometry::Vec3D ZAxis=
	IParam.getDefValue<Geometry::Vec3D>(Geometry::Vec3D(0,0,1),"sdefVec",2);
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
      
      if (sdefType=="Spectrum")                       // Spectrum beam
	{
	  const std::string fileName=IParam.getValue<std::string>
	    ("sdefType",sdefIndex,1);
	  //	  sName=SDef::createWeightedSource(inputMap,FC,linkIndex,fileName);
	}
      else if (sdefType=="Beam" || sdefType=="beam")
	sName=SDef::createBeamSource(inputMap,"beamSource",FC,linkIndex);

      
      // else if (sdefType=="external" || sdefType=="External" ||
      // 	       sdefType=="source" || sdefType=="Source")
      // 	eName=SDef::createFlukaSource(inputMap,"phitsSource",FC,linkIndex);
	 
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

  

  
} // NAMESPACE SDef
