/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/MainProcess.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/format.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "support.h"
#include "defaultConfig.h"
#include "DefUnits.h"

namespace mainSystem
{

void 
setDefUnits(FuncDataBase& Control,
	    inputParam& IParam)
  /*!
    Based on the defaultConf set up the model
    \param Control :: FuncDataBase
  */
{
  ELog::RegMethod RegA("DefUnits","setDefUints");

  defaultConfig A("");
  if (IParam.flag("defaultConfig"))
    {
      const std::string Key=IParam.getValue<std::string>("defaultConfig");
      if (Key=="TS1MarkIV")
	setTS1MarkIV(A);
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
setTS1MarkIV(defaultConfig& A)
  /*!
    Default configureation for MarkIV
    \param A :: Paramter for default config
   */
{
  ELog::RegMethod RegA("DefUnits[F]","setTS1MarkII");

  A.setOption("targetType","t1CylFluxTrap");
  A.setOption("CH4PreType","Flat");

  A.setVar("sdefZOffset",0.0);
  A.setVar("TriModPoisonThick",0.005);
  A.setVar("TriModPCladThick",0.3);
  A.setVar("TriModPoisonMat","Gadolinium");
  A.setVar("TriModXYangle",-95.0); 
  A.setVar("TriModXStep",0.0);
  A.setVar("TriModYStep",0.0);
  A.setVar("TriFlightAAngleXY1",30.0);
  A.setVar("TriFlightAAngleXY2",0.0);
  A.setVar("TriFlightAXStep",-2.0);
  A.setVar("TriFlightAWidth",33.0);
  A.setVar("TriFlightBAngleXY2",5.0);
  A.setVar("TriFlightBWidth",23.0);

  A.setVar("TriModNCorner",3);  
  A.setVar("TriModCorner1",Geometry::Vec3D(2.1,23.88,0)); 
  A.setVar("TriModCorner2",Geometry::Vec3D(11.63,-13.82,0)); 
  A.setVar("TriModCorner3",Geometry::Vec3D(-11.61,13.79,0.0)); 
  A.setVar("TriModAbsolute1",1);
  A.setVar("TriModAbsolute2",1);
  A.setVar("TriModAbsolute3",1);

  A.setVar("TriModNInnerUnits",1); 
  A.setVar("TriModInner1Corner1",Geometry::Vec3D(-5.86,10.95,0.0)); 
  A.setVar("TriModInner1Corner2",Geometry::Vec3D(1.41,16.12,0.0)); 
  A.setVar("TriModInner1Corner3",Geometry::Vec3D(9.04,-8.4,0)); 
  A.setVar("TriModInner1Absolute1",1);
  A.setVar("TriModInner1Absolute2",1);
  A.setVar("TriModInner1Absolute3",1);

  A.setVar("TriFlightASideIndex",1);
  A.setVar("TriFlightBSideIndex",2);

  A.setVar("TriModNInnerLayers",4); 
  A.setVar("TriModInnerThick0",0.3); 
  A.setVar("TriModInnerThick1",0.005); 
  A.setVar("TriModInnerThick2",0.3); 
  A.setVar("TriModInnerMat0","Aluminium");
  A.setVar("TriModInnerMat1","Gadolinium");
  A.setVar("TriModInnerMat2","Aluminium");
  A.setVar("TriModInnerMat3","Void");

  return;
}


}
