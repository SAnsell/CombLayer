/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/flukaTallySelector.cxx
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"

#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimFLUKA.h"


#include "flukaTallyModification.h"



void
tallyModification(SimFLUKA& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Applies a large number of modifications to the tally system
    \param System :: SimFLUKA holding tallies to change
    \param IParam :: Parameters
  */
{
  ELog::RegMethod RegA("flukaTallySelector[F]","tallyModification");
  const size_t nP=IParam.setCnt("TMod");
	  	
  for(size_t i=0;i<nP;i++)
    {
      std::vector<std::string> StrItem;
      // This is enforced a >1
      const size_t nV=IParam.itemCnt("TMod",i);
      const std::string key=
	IParam.getValue<std::string>("TMod",i,0);
      // why this:
      for(size_t j=1;j<nV;j++)
	StrItem.push_back
	  (IParam.getValue<std::string>("TMod",i,j));

      if(key=="help")
	{
	  ELog::EM<<"TMod Help "
	    "  -- userName :: tallyName/Number ExternalaName \n"
	    "  -- ascii :: tallyName/Number \n"
	    "  -- binary :: tallyName/Number \n"
	    "  -- particle ::tallyName/Number particle \n"
	    "  -- auxParticle tallyName/Number particle \n"
	    "  -- doseType : tallyName/Number {particle} [newtype] \n"
	    "  -- energy : tallyName/Number Emin Emax NPts LinearFlag  \n"
	    "  -- angle : tallyName/Number Amin Amax NPts LogFlag  \n"
		  <<ELog::endBasic
		  <<ELog::endErr;
          return;
	}
      
      if(key=="ascii")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for doseType");
	  flukaSystem::setAsciiOutput(System,tName);
        }
      else if(key=="binary")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for doseType");
	  flukaSystem::setBinaryOutput(System,tName);
        }
      else if(key=="doseType")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for "+key);
	  const std::string PT=IParam.getValueError<std::string>
	    ("TMod",i,2,"No particle for "+key);
	  const std::string DT=IParam.getValueError<std::string>
	    ("TMod",i,3,"No standard for "+key);
          flukaSystem::setDoseType(System,tName,PT,DT);
        }
      else if(key=="userName")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for "+key);
	  const std::string externalName=IParam.getValueError<std::string>
	    ("TMod",i,2,"No external for "+key);
          flukaSystem::setUserName(System,tName,externalName);
        }
      else if(key=="auxParticle")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for "+key);
	  const std::string PT=IParam.getValueError<std::string>
	    ("TMod",i,2,"Particle for "+key);
          flukaSystem::setAuxParticle(System,tName,PT);
        }
      else if(key=="energy")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for "+key);
	  const double EA=IParam.getValueError<double>
	    ("TMod",i,2,"Emin for "+key);
	  const double EB=IParam.getValueError<double>
	    ("TMod",i,3,"Emax for "+key);
	  const size_t NE=IParam.getValueError<size_t>
	    ("TMod",i,4,"NPTS for "+key);
	  const std::string EFlag=
	    IParam.getDefValue<std::string>("log","TMod",i,5);
	  const bool lFlag=(StrFunc::toLowerString(EFlag)=="log" ? 1 : 0);
          flukaSystem::setEnergy(System,tName,EA,EB,NE,lFlag);
        }
      else if(key=="angle")
        {
	  const std::string tName=IParam.getValueError<std::string>
	    ("TMod",i,1,"No tally name for "+key);
	  const double AA=IParam.getValueError<double>
	    ("TMod",i,2,"Amin for "+key);
	  const double AB=IParam.getValueError<double>
	    ("TMod",i,3,"Amax for "+key);
	  const size_t NA=IParam.getValueError<size_t>
	    ("TMod",i,4,"NPTS for "+key);

	  const int AFlag=IParam.getDefValue<int>(0,"TMod",i,5);
          flukaSystem::setAngle(System,tName,AA,AB,NA,AFlag);
        }
      else
	ELog::EM<<"Currently no modification possible for:"<<key<<ELog::endErr;
    }
  return;
}


