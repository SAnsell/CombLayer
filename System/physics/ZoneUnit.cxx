/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ZoneUnit.cxx
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
#include "MapRange.h"
#include "support.h"
#include "permSort.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "ZoneUnit.h" 


namespace physicsSystem
{

template<typename T>
ZoneUnit<T>::ZoneUnit() 
  /// Constructor
{}

template<typename T>  
ZoneUnit<T>::ZoneUnit(const ZoneUnit<T>& A) :
  Zones(A.Zones),ZoneData(A.ZoneData)
  /*!
    Copy constructor
    \param A :: ZoneUnit to copy
  */
{}

template<typename T>
ZoneUnit<T>&
ZoneUnit<T>::operator=(const ZoneUnit<T>& A)
  /*!
    Assignment operator
    \param A :: ZoneUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Zones=A.Zones;
      ZoneData=A.ZoneData;
    }
  return *this;
}

template<typename T>
size_t
ZoneUnit<T>::findItem(const int cellN) const
  /*!
    Returns the index + 1 if cellN is found
    \param cellN :: Number to check
    \return 0 if nothing found / index+1 
   */
{
  std::vector<MapSupport::Range<int>>::const_iterator vc=
    std::lower_bound(Zones.begin(),Zones.end(),
                     MapSupport::Range<int>(cellN));
  if (vc!=Zones.end())
    {
      const size_t index=static_cast<size_t>(vc-Zones.begin());
      if (vc->valid(cellN))
        return index+1;
    }
  return 0;
}

template<typename T>
MapSupport::Range<int>
ZoneUnit<T>::createMapRange(std::vector<int>& CellList)
  /*!
    Convert the first range inthe vector into a Range
    then remove the component
  */
{
  ELog::RegMethod RegA("ZoneUnit","createMapRange");
  
  if (CellList.empty())
    throw ColErr::EmptyValue<int>("CellList");

  int ACell(CellList.front());
  int BCell(ACell);
    
  std::vector<int>::iterator vc=CellList.begin()+1;
  while(vc!=CellList.end() &&
	*vc== BCell+1)
    {
      BCell++;  // care here because BCell must not increase if vc==end
      vc++;
    }
  CellList.erase(CellList.begin(),vc);
  return MapSupport::Range<int>(ACell,BCell);
    
}
  
template<typename T>
bool
ZoneUnit<T>::procZone(const objectGroups& OGrp,
		      std::vector<std::string>& StrItem)
  /*!
    Process the zone information
    \param StrItem :: List of item from the input [Used items erased]
    \return true on success 
  */
{
  ELog::RegMethod RegA("ZoneUnit","procZone");

  const size_t NS(StrItem.size());
  long int cut(0);
  int cNum,dNum;
  if (NS>=1 && (StrItem[0]=="All" || StrItem[0]=="all"))
    {
      Zones.push_back(MapSupport::Range<int>(0,100000000));
      cut=1;
    }
  else if (NS>=2 && (StrItem[0]=="Object" || StrItem[0]=="object"))
    {
      std::vector<int> cellN=OGrp.getObjectRange(StrItem[1]);
      while(!cellN.empty())
	{
	  Zones.push_back(createMapRange(cellN));
	}
      cut=2;
    }
  else if (NS>=2 && StrItem[0]=="Cells")
    {
      size_t index(1);
      while(index<NS &&
	    StrFunc::convert(StrItem[index],cNum))
	{
	  Zones.push_back(MapSupport::Range<int>(cNum,cNum));
	  index++;
	}
      if (index!=1) cut=static_cast<long int>(index);
    }
  else if (NS>=3 && StrItem[0]=="Range")
    {
      if (StrFunc::convert(StrItem[1],cNum) &&
	  StrFunc::convert(StrItem[2],dNum) )
	{
	  int ACell=(OGrp.isActive(cNum) ? cNum : 0);
	  for(int CN=cNum+1;CN<dNum;CN++)
	    {
	      if (ACell && !OGrp.isActive(CN))
		{
		  Zones.push_back(MapSupport::Range<int>(ACell,CN-1));
		  ACell=0;
		}
	      else if (!ACell && OGrp.isActive(CN))
		{
		  ACell=CN;
		}
	    }
	  if (ACell)
	    Zones.push_back(MapSupport::Range<int>(ACell,dNum));
	  cut=3;
	}
    }
  
  if (!cut)
    return 0;
      
  StrItem.erase(StrItem.begin(),StrItem.begin()+cut);
  return 1;
}

template<typename T>
void
ZoneUnit<T>::sortZone()
  /*!
    This is going to sort and concaternate the zones
  */
{
  ELog::RegMethod RegA("ZoneUnit","sortZone");

  if (Zones.empty()) return;
  if (ZoneData.empty())
    {
      std::sort(Zones.begin(),Zones.end());
      return;
    }
  
  if (Zones.size()!=ZoneData.size())
    throw ColErr::MisMatch<size_t>(Zones.size(),ZoneData.size(),
                                   "Zone!=ZoneData");

  
  const std::vector<size_t> ZIndex=
    mathSupport::sortPermutation
    (Zones,[](const MapSupport::Range<int>& a,const MapSupport::Range<int>& b)
     { return (a<b); } );

  mathSupport::applyPermutation(Zones,ZIndex);
  mathSupport::applyPermutation(ZoneData,ZIndex);
  
  return;
}

template<typename T>
void
ZoneUnit<T>::addData(const T& Value)
  /*!
    Add a specific value to the back of ZoneData
    \param Value :: new data value 
  */
{
  ZoneData.push_back(Value);
  return;
}

template<typename T>
bool
ZoneUnit<T>::inRange(const int cellN,T& Value) const
  /*!
    Given a cellN find the ZoneUnit that overlap and 
    return the value.
    \param cellN :: Number to search
    \param Value :: Value that is set if cellN valid
    \return true if in a ZoneUnit
   */
{
  const size_t index=findItem(cellN);
  if (index)
    {
      Value=ZoneData[index-1];
      return 1;
    }
  return 0;
}
  
/// \cond TEMPLATE
  
template class ZoneUnit<double>;
  
/// \endcond TEMPLATE


}  // NAMESPACE physicsSystem
