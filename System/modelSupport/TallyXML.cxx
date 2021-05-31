/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/TallyXML.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list> 
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "textTally.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "XMLload.h"
#include "XMLattribute.h"
#include "XMLobject.h"
#include "XMLgroup.h"
#include "XMLcollect.h"
#include "XMLnamespace.h"

namespace tallySystem
{
  
void
addXMLtally(Simulation& System,
	    const std::string& FName)
  /*!
    Tally from an XML file
    \param System :: Simuation object 
    \param FName :: Simuation object 
  */
{
  ELog::RegMethod RegA("tallySystem","addXMLtally");

  XML::XMLcollect CO;
  if (CO.loadXML(FName))
    {
      ELog::EM<<"Failed to load XML tally information:"<<FName<<ELog::endErr;
      return;
    }

  // Parse for variables:
  XML::XMLobject* AR=CO.findObj("tally");

  while(AR)
    {
      const int tNumber=AR->getAttribute<int>("number",9);
      const int rFlag=AR->getAttribute<int>("rotate",0);
      std::vector<std::string> Keys;
      std::vector<std::string> Lines;
      XML::splitObjGroup(AR,Keys,Lines);
      Tally* TX(0);
      try
	{
	  // TX=tallyFactory::Instance()->createTally(tNumber);
	}
      catch (ColErr::InContainerError<int>&)
	{
	  ELog::EM<<"Adding text only tally :"<<tNumber
		  <<"["<<rFlag<<"]"<<ELog::endCrit;
	  TX=new textTally(tNumber);
	}
      TX->setActive(1);

      for(size_t i=0;i<Keys.size();i++)
	if (Keys[i]=="line" &&
	    TX->addLine(Lines[i]))
	  {
	    delete TX;
	    TX=0;
	    break;
	  }
      
      SimMCNP* SM=dynamic_cast<SimMCNP*>(&System);
      if (SM)
	SM->addTally(*TX);

      delete TX;
      CO.deleteObj(AR);   
      AR=CO.findObj("tally");
    }
  return;
}

}  // NAMESPACE tallySystem
