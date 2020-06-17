/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   maxviBuildInc/DefUnitsMaxIV.cxx
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
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "defaultConfig.h"
#include "DefUnitsMaxIV.h"

namespace mainSystem
{

void
setDefUnits(FuncDataBase& Control,inputParam& IParam)
  /*!
    Based on the defaultConf set up the model
    \param Control :: FuncDataBase
    \param IParam :: input system
  */
{
  ELog::RegMethod RegA("DefUnitsMaxIV[F]","setDefUnits");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const std::string Key=IParam.getValue<std::string>("defaultConfig");

      std::vector<std::string> LItems=
	IParam.getObjectItems("defaultConfig",0);
      const std::string sndItem=(LItems.size()>1) ? LItems[1] : "";
      const std::string extraItem=(LItems.size()>2) ? LItems[2] : "";

      if (Key=="Linac" || Key=="LINAC")
	{
	  LItems[0]="LINAC";   // ensure we have case correctness
	  setMaxIVLinac(A,LItems);
	}
      else if (Key=="Single")
	setMaxIVSingle(A,LItems);
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  Linac : Everything that works"<<ELog::endDiag;
	  ELog::EM<<"  Single  beamLine : Single beamline "<<ELog::endDiag;
	  throw ColErr::ExitAbort("Iparam.defaultConfig");
	}
      else
	{
	  ELog::EM<<"Unknown Default Key ::"<<Key<<ELog::endDiag;
	  throw ColErr::InContainerError<std::string>
	    (Key,"Iparam.defaultConfig");
	}
      // ???

      A.process(Control,IParam);
    }
  return;
}

void
setMaxIVSingle(defaultConfig& A,
	       const std::vector<std::string>& LItems)

  /*!
    Default configuration for MaxIV for testing single beamlines
    \param A :: Paramter for default config
    \param LItems :: single selection
   */
{
  ELog::RegMethod RegA("DefUnitsMaxIV[F]","setMaxIVSingle");

  typedef std::map<std::string,std::string> MapTYPE;
  static const MapTYPE beamDef=
    {
      { "BALDER", "World 0"},
      { "COSAXS", "World 0"},
      { "SOFTIMAX", "World 0"},
      { "DANMAX", "World 0"},
      { "FLEXPES", "World 0"},
      { "FORMAX", "World 0"},
      { "MAXPEEM", "World 0"},
      { "SPECIES", "World 0"},
      { "LINAC", "World 0"},
      { "RING1", "World 0"},
      { "RING3", "World 0"},
      { "R1RING", "World 0"},
      { "R3RING", "World 0"}
    };

  size_t beamLineIndex(0);

  for(const std::string& beamItem : LItems)
    {
      if (beamItem!="Single")
	{
	  MapTYPE::const_iterator mc=beamDef.find(beamItem);

          if (mc!=beamDef.end())
            {
	      A.setMultiOption("beamlines",beamLineIndex,
			       beamItem+" "+mc->second);
	      beamLineIndex++;
            }
          else
            throw ColErr::InContainerError<std::string>(beamItem,"BeamItem");
        }
    }
  return;
}

void
setMaxIVLinac(defaultConfig& A,
	      const std::vector<std::string>& LItems)
  /*!
    Default configuration for MaxIV for linac units
    \param A :: Paramter for default config
    \param LItems :: single selection
   */
{
  ELog::RegMethod RegA("DefUnitsMaxIV[F]","setMaxIVLinac");

  typedef std::map<std::string,std::string> MapTYPE;
  static const MapTYPE unitDef=
    {
     { "LINAC", "World 0"},
     { "L2SPF", "World 0"},
     { "TDC", "World 0"},
     { "L2SPFsegment1", "World 0"},
     { "L2SPFsegment2", "World 0"},
     { "L2SPFsegment3", "World 0"},
     { "L2SPFsegment4", "World 0"},
     { "L2SPFsegment5", "World 0"},
     { "L2SPFsegment6", "World 0"},
     { "L2SPFsegment7", "World 0"},
     { "L2SPFsegment8", "World 0"},
     { "L2SPFsegment9", "World 0"},
     { "L2SPFsegment10", "World 0"},
     { "L2SPFsegment11", "World 0"},
     { "L2SPFsegment12", "World 0"},
     { "L2SPFsegment13", "World 0"},
     { "TDCsegment14", "World 0"},
     { "TDCsegment15", "World 0"},
     { "TDCsegment16", "World 0"},
     { "TDCsegment17", "World 0"},
     { "TDCsegment18", "World 0"},
     { "TDCsegment19", "World 0"},
     { "TDCsegment20", "World 0"},
     { "TDCsegment21", "World 0"},
     { "TDCsegment22", "World 0"},
     { "TDCsegment23", "World 0"},
     { "TDCsegment24", "World 0"},
     { "TDCsegment25", "World 0"}
    };

  size_t unitIndex(0);
  for(const std::string& compItem : LItems)
    {
      MapTYPE::const_iterator mc=unitDef.find(compItem);

      if (mc==unitDef.end())
	throw ColErr::InContainerError<std::string>(compItem,"CompItem");
      A.setMultiOption("beamlines",unitIndex,compItem+" "+mc->second);
      unitIndex++;
    }

  return;
}

} // NAMESPACE mainSystem
