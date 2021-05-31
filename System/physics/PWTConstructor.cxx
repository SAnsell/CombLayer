/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PWTConstructor.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "Triple.h"
#include "support.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkSupport.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "ModeCard.h"

#include "PhysCard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"
#include "PhysicsCards.h"

#include "PWTControl.h"
#include "PWTConstructor.h" 


namespace physicsSystem
{

PWTConstructor::PWTConstructor() 
  /// Constructor
{}

bool
PWTConstructor::procZone(const objectGroups& OGrp,
			 std::vector<std::string>& StrItem)
  /*!
    Process the zone information
    \param StrItem :: List of item from the input [Used items erased]
    \return true on success 
  */
{
  ELog::RegMethod RegA("ExtConstuctor","procZone");

  const size_t NS(StrItem.size());
  long int cut(0);
  int cNum,dNum;
  if (NS>=1 && (StrItem[0]=="All" || StrItem[0]=="all"))
    {
      Zones.setItems(OGrp.getActiveCells());
      cut=1;
    }
  else if (NS>=2 && (StrItem[0]=="Object" || StrItem[0]=="object"))
    {
      const std::vector<int> CVec=OGrp.getObjectRange(StrItem[1]);

      if (CVec.empty())
	throw ColErr::InContainerError<std::string>(StrItem[1],"Object name");
      Zones.addItem(CVec);
      cut=2;
    }
  else if (NS>=2 && StrItem[0]=="Cells")
    {
      size_t index(1);
      while(index<NS &&
	    StrFunc::convert(StrItem[index],cNum))
	{
	  Zones.addItem(cNum);
	  index++;
	}
      if (index!=1) cut=static_cast<long int>(index);
    }
  else if (NS>=3 && StrItem[0]=="Range")
    {
      if (StrFunc::convert(StrItem[1],cNum) &&
	  StrFunc::convert(StrItem[2],dNum) )
	{
	  Zones.addItem(cNum,dNum);
	  cut=3;
	}
    }
  
  if (!cut)
    return 0;
      
  StrItem.erase(StrItem.begin(),StrItem.begin()+cut);
  return 1;
}

bool
PWTConstructor::getVector(const objectGroups& OGrp,
			  const std::vector<std::string>& StrItem,
			  const size_t index,
			  Geometry::Vec3D& Pt)
  /*!
    Get a vector based on a StrItem / iether using a
    a name unit or a value.
    \param StrItem :: List of strings
    \param index :: Place to start in list
    \param Pt :: Point found
    \return 1 on success / 0 on failure
   */
{
  ELog::RegMethod RegA("PWTConstructor","getVector");

  const size_t NS(StrItem.size());
  // Simple Vec3D(a,b,c)
  if (NS>=index && StrFunc::convert(StrItem[index],Pt) )
    return 1;

  // Test of FixedPoint link
  if (NS >= index+2)
    {
      Geometry::Vec3D YAxis;  
      if (attachSystem::getAttachPoint
	  (OGrp,StrItem[index],StrItem[index+1],Pt,YAxis))
	return 1;
    }
  
  // Simple vector
  if (NS >= index+3 && StrFunc::convert(StrItem[index],Pt[0])
	   && StrFunc::convert(StrItem[index+1],Pt[1])
	   && StrFunc::convert(StrItem[index+2],Pt[2]) )
    return 1;

  return 0;
}

  
bool
PWTConstructor::procType(std::vector<std::string>& StrItem,
			 PWTControl& PWT)
  /*!
    Process the Type information
    \param StrItem :: List of item from the input [Used items erased]
    \param PWT :: Control card to place data [and zone]
    \return true on success 
  */
{
  ELog::RegMethod RegA("ExtConstuctor","procType");

  const size_t NS(StrItem.size());
  if (!NS) return 0;

  double scalar;

  if (NS>=2 && (StrItem[0]=="simple") &&
      StrFunc::convert(StrItem[1],scalar))
    {
      const std::vector<int> ZVec=Zones.getAllCells();
      for(const int rUnit : ZVec)
	PWT.setUnit(rUnit,scalar);
      return 1;
    }
  return 0;
}


void
PWTConstructor::processUnit(SimMCNP& System,
			    const mainSystem::inputParam& IParam,
			    const size_t Index) 
/*!
    Add pwt component 
    \param System :: Simulation
    \param IParam :: Main input parameters
    \param Index :: index of the -wPWT card
   */
{
  ELog::RegMethod RegA("PWTConstructor","processPoint");

  physicsSystem::PhysicsCards& PC=System.getPC();
  
  const size_t NParam=IParam.itemCnt("wPWT",Index);
  if (NParam<1)
    throw ColErr::IndexError<size_t>(NParam,2,"Insufficient items wPWT");
  std::vector<std::string> StrItem;
  
  // Get all values:
  for(size_t j=0;j<NParam;j++)
    StrItem.push_back
      (IParam.getValue<std::string>("wPWT",Index,j));

  if (StrItem[0]=="help" || StrItem[0]=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  
  if (!procZone(System,StrItem))
    throw ColErr::InvalidLine
      ("procZone ==> StrItems","-wPWT "+IParam.getFull("wPWT",Index),0);	


  PWTControl& PWT=PC.getPWTCard();
    
  if (!procType(StrItem,PWT))
    throw ColErr::InvalidLine
      ("procType ==> StrItems","-wPWT "+IParam.getFull("wPWT",Index),0);	

  return;
}

void
PWTConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX :: Output stream
  */
{

    OX<<"-wPWT ZONE : Value \n"
      " :: \n"
      "   All / object [objectName] / Cells {numbers}(s) / [cellNumber] \n"
      "TYPE :: \n"
      "   simple [value] \n"
      "       -- vias to the point\n";
  return;
}


}  // NAMESPACE physicsSystem
