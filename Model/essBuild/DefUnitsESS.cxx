/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DefUnitsESS.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "inputParam.h"
#include "support.h"
#include "stringCombine.h"
#include "defaultConfig.h"
#include "DefUnitsESS.h"

namespace mainSystem
{

void 
setDefUnits(FuncDataBase& Control,
	    inputParam& IParam)
  /*!
    Based on the defaultConf set up the model
    \param Control :: FuncDataBase
    \param IParam :: input system
  */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setDefUnits");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const size_t ICnt=IParam.itemCnt("defaultConfig",0);
      const std::string Key=IParam.getValue<std::string>("defaultConfig");

      const std::string sndItem=(ICnt>1) ? 
	IParam.getValue<std::string>("defaultConfig",1) : "";
      const std::string extraItem=(ICnt>2) ? 
	IParam.getValue<std::string>("defaultConfig",2) : "";
      const int filled=(ICnt>3) ? 
	IParam.getValue<int>("defaultConfig",3) : 0;

      if (Key=="Main")
	setESS(A);
      else if (Key=="Full")
	setESSFull(A);
      else if (Key=="PortsOnly")
	setESSPortsOnly(A,sndItem,extraItem);
      else if (Key=="Single")
	setESSSingle(A,sndItem,extraItem,filled);
      else if (Key=="neutronics")
	setESSNeutronics(A);
	
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  Main : Everything that works"<<ELog::endDiag;
	  ELog::EM<<"  Full : Beamline on every port"<<ELog::endDiag;
	  ELog::EM<<"  PortsOnly [lower/upper] : Nothing beyond beamport "<<ELog::endDiag;
	  ELog::EM<<"  Single  beamLine : Single beamline [for BL devel] "
		  <<ELog::endDiag;
	  ELog::EM<<"  neutronics : configuration for neutronics calculations " << ELog::endDiag;
	  throw ColErr::ExitAbort("Iparam.defaultConfig");	  
	}
      else 
	{
	  ELog::EM<<"Unknown Default Key ::"<<Key<<ELog::endDiag;
	  throw ColErr::InContainerError<std::string>
	    (Key,"Iparam.defaultConfig");
	}
      A.process(Control,IParam);
    }
  return;
}

void
setESSFull(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");

  A.setOption("lowMod","Butterfly");
  A.setOption("topMod","Butterfly");

  const std::map<std::string,std::string> beamDef=
    {
      //      {"NMX","G4BLine18"},
      //      {"SHORTDREAM","G4BLine9"},
      //      {"SHORTDREAM2","G4BLine1"},
      //      {"SHORTODIN","G4BLine7"},
      //      {"DREAM","G4BLine17"},
      //      {"VOR","G4BLine3"},   // also 17  
      //      {"LOKI","G4BLine5"},
      //      {"ODIN","G4BLine15"}
      {"VOR","G4BLine1"},   // also 17
      {"SHORTDREAM","G4BLine3"},
      {"SHORTODIN","G4BLine5"},
      {"LOKI","G4BLine7"},
      {"SHORTDREAM2","G4BLine9"},
      {"DREAM","G4BLine17"},
      {"CSPEC","G4BLine15"},
      {"VESPA","G4BLine11"},
      {"ODIN","G4BLine15"}
    };     
  const std::set<std::string> beamFilled=
    {"NMX","CSPEC","DREAM","VESPA","VOR","SHORTDREAM","SHORTDREAM2","LOKI"};

  size_t index(0);
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=beamDef.begin();mc!=beamDef.end();mc++)
    {
      A.setMultiOption("beamlines",index,mc->second+" "+mc->first);
      A.setVar(mc->second+"Active",1);
      if (beamFilled.find(mc->first)!=beamFilled.end())
	A.setVar(mc->second+"Filled",1);
      index++;
    }
  return;
}

void
setESSPortsOnly(defaultConfig& A,const std::string& lvl,
                const std::string& lvlExtra)
  /*!
    Default configuration for ESS for beamports only
    \param A :: Paramter for default config
    \param lvl :: level value (lower/uppper)
    \param lvlExtra :: extra level value (lower/uppper)
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");

  static const std::map<std::string,std::string>
    nameBay({
        {"lower","G1BLine"},
        {"upper","G4BLine"},
        {"farUpper","G3BLine"},
        {"farLower","G2BLine"}
      });
  
  A.setOption("lowMod","Butterfly");

  int defaultFlag(0);
  std::string GNum;
  std::map<std::string,std::string>::const_iterator mc=
    nameBay.find(lvlExtra);
  if (mc!=nameBay.end())
    {
      for(size_t i=0;i<21;i++)
        A.setVar(mc->second+StrFunc::makeString(i+1)+"Active",1);
      defaultFlag=1;
      GNum=mc->second+" ";
    }
  // and take default:
  mc=nameBay.find(lvl);
  if (mc!=nameBay.end())
    {
      for(size_t i=0;i<21;i++)
        A.setVar(mc->second+StrFunc::makeString(i+1)+"Active",1);
      defaultFlag=1;
      GNum+=mc->second;
    }
  if (!defaultFlag)
    {
      GNum="G4BLine";
      for(size_t i=0;i<21;i++)
        A.setVar("G4BLine"+StrFunc::makeString(i+1)+"Active",1);
    }
      
  ELog::EM<<"Port Only on sectors:"<<GNum<<ELog::endDiag;
  return;
}

void
setESSSingle(defaultConfig& A,
	     const std::string& beamItem,
	     const std::string& portItem,
	     int filled)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param beamItem :: Additional value for beamline name
    \param portItem :: Additional value for port number/item
    \param active :: Active flag
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSSingle");

  A.setOption("lowMod","Butterfly");
  const std::map<std::string,std::string> beamDefNotSet=
    {{"BIFROST","G4BLine4"},
     {"MIRACLES","G4BLine5"},
     {"MAGIC","G1BLine6"},
     {"TREX","G4BLine7"},
     {"HEIMDAL","G4BLine8"},
     {"SLEIPNIR","G4BLine13"},   // N9
     {"ANNI","G3BLine3"},        // E3
     {"SURFSCATTER","G3BLine8"},        // E8
     {"SKADI","G3BLine5"}         // E5
    };     
  const std::map<std::string,std::string> beamDef=
    {{"NMX","G4BLine1"},        // W1
     {"BIFROST","G4BLine4"},    // W4
     {"SHORTDREAM","G4BLine17"},
     {"SHORTODIN","G1BLine4"},
     {"DREAM","G3BLine19"},     // S3
     {"CSPEC","G4BLine3"},      // W3
     {"VESPA","G3BLine7"},      // E7
     {"VOR","G3BLine10"},       // E11  [CHANGED TO FIT]
     {"SIMPLE","G4BLine17"},
     {"LOKI","G4BLine17"},      // N7
     {"ODIN","G2BLine2"},       // Lower S2
     {"ESTIA","G3BLine2"},      // E2
     {"FREIA","G4BLine15"},     // N5
     {"BEER","G4BLine2"}        // W2
    };     
  const std::set<std::string> beamFilled=
    {"BEER","BIFROST","CSPEC","DREAM","FREIA","LOKI",
     "NMX","VESPA","VOR","SHORTDREAM"};
  
  std::map<std::string,std::string>::const_iterator mc=
    beamDef.find(beamItem);
  if (filled<0)  // deactivation if set
    filled=0;
  else if (!filled && beamFilled.find(beamItem)!=beamFilled.end())
    filled=1;
    
  if (mc!=beamDef.end())
    {
      if (portItem.empty())
	{
	  A.setMultiOption("beamlines",0,mc->second+" "+beamItem);
	  A.setVar(mc->second+"Active",1);
	  if (filled)
	    A.setVar(mc->second+"Filled",1);
	}
      else
	{
	  A.setMultiOption("beamlines",0,portItem+" "+beamItem);
	  A.setVar(portItem+"Active",1);
	  if (filled)
	    A.setVar(portItem+"Filled",1);
	}
    }
  else
    throw ColErr::InContainerError<std::string>(beamItem,"BeamItem");

  ELog::EM<<"TEST of "<<beamItem<<" Only "<<ELog::endDiag;
  return;
}

void
setESSNeutronics(defaultConfig& A)

  /*!
    Default configuration for ESS for testing single beamlines
    for building
    \param A :: Paramter for default config
    \param beamItem :: Additional value for beamline name
    \param portItem :: Additional value for port number/item
    \param active :: Active flag
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESSSingle");

  A.setOption("matDB", "neutronics");
  A.setOption("lowMod", "Butterfly");
  A.setOption("bunker", "noPillar");
  return;
}

void
setESS(defaultConfig& A)
  /*!
    Default configuration for ESS
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnitsESS[F]","setESS");


  A.setOption("lowMod","Butterfly");
  A.setOption("topMod","Butterfly");

  const std::map<std::string,std::string> beamDef=
    {
      {"NMX","G4BLine17"},
      {"DREAM","G4BLine14"},
      {"CSPEC","G4BLine13"},
      {"VOR","G4BLine7"},   // also 17  
      {"LOKI","G4BLine4"},
      {"ODIN","G4BLine19"}
    };     
  const std::set<std::string> beamFilled=
    {"NMX","CSPEC","DREAM","VOR","LOKI"};

  size_t index(0);
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=beamDef.begin();mc!=beamDef.end();mc++)
    {
      A.setMultiOption("beamlines",index,mc->second+" "+mc->first);
      A.setVar(mc->second+"Active",1);
      if (beamFilled.find(mc->first)!=beamFilled.end())
	A.setVar(mc->second+"Filled",1);
      index++;
    }
   
  return;
}

  
} // NAMESPACE mainSystem
