/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/DefUnitsESS.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
      const std::string Key=IParam.getValue<std::string>("defaultConfig");
      if (Key=="Main")
	setESS(A);
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  Main  "<<ELog::endDiag;
	  throw ColErr::InContainerError<std::string>
	    (Key,"Iparam.defaultConfig");	  
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
  A.setOption("lowModFlowGuide","On");
  A.setOption("topModFlowGuide","On");
  A.setOption("lowWaterDisk","On");
  A.setOption("topWaterDisk","On");
  A.setOption("topWaterDisk","On");
  A.setMultiOption("beamlines",0,"G1BLine1 ODIN");
  A.setMultiOption("beamlines",1,"G1BLine3 LOKI");
  A.setMultiOption("beamlines",2,"G1BLine5 NMX");
     
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
  A.setMultiOption("beamlines",0,"G1BLine1 ODIN");
  A.setMultiOption("beamlines",1,"G1BLine3 LOKI");
  A.setMultiOption("beamlines",2,"G1BLine5 NMX");
     
  return;
}

} // NAMESPACE mainSystem
