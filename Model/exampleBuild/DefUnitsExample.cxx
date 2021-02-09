/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxviBuildInc/DefUnitsExample.cxx
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
 * MERCHANTABILITY or FITNExample FOR A PARTICULAR PURPOSE.  See the
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
#include "DefUnitsExample.h"

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
  ELog::RegMethod RegA("DefUnitsExample[F]","setDefUnits");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const std::string Key=IParam.getValue<std::string>("defaultConfig");
      
      std::vector<std::string> LItems=
	IParam.getObjectItems("defaultConfig",0);
      const std::string sndItem=(LItems.size()>1) ? LItems[1] : "";
      const std::string extraItem=(LItems.size()>2) ? LItems[2] : "";

      if (Key=="DIPOLE")
	setExampleEX1(A,LItems);
      else if (Key=="VACTUBE")
	setLinacVacTube(A,LItems);
      else if (Key=="help")
	{
	  ELog::EM<<"Options : "<<ELog::endDiag;
	  ELog::EM<<"  EX1 : Example 1"<<ELog::endDiag;
	  ELog::EM<<"  VACTUBE : Linac Vacuum Tube"<<ELog::endDiag;
	  throw ColErr::ExitAbort("Iparam.defaultConfig");	  
	}
    }
  A.setOption("sdefType","Point");        
  
  A.process(Control,IParam);
  return;
}

void
setExampleEX1(defaultConfig& A,
	      const std::vector<std::string>& LItems)
  /*!
    Placeholder for EX1
    \param A :: defaultConfig to set
    \param LItems :: extra items
  */
{
  A.setMultiOption("Model",0,"EX1");
  return;
}

void
setLinacVacTube(defaultConfig& A,
	      const std::vector<std::string>& LItems)
  /*!
    Placeholder for Linac Vac Tube
    \param A :: defaultConfig to set
    \param LItems :: extra items
  */
{
  A.setMultiOption("Model",0,"VACTUBE");
  return;
}


} // NAMESPACE mainSystem
