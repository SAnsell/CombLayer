/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/ZoneUnit.cxx
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MapRange.h"
#include "Triple.h"
#include "support.h"
#include "mathSupport.h"
#include "stringCombine.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "Simulation.h"
#include "inputParam.h"
#include "ZoneUnit.h" 


namespace physicsSystem
{

template<typename T>
ZoneUnit<T>::ZoneUnit() 
  /// Constructor
{}

template<typename T>
bool
ZoneUnit<T>::procZone(std::vector<std::string>& StrItem)
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
  else if (NS>=2 && StrItem[0]=="Object")
    {
      const ModelSupport::objectRegister& OR= 
	ModelSupport::objectRegister::Instance();
      const int cellN=OR.getCell(StrItem[1]);
      const int rangeN=OR.getRange(StrItem[1]);
      if (cellN==0)
	throw ColErr::InContainerError<std::string>(StrItem[1],"Object name");
      Zones.push_back(MapSupport::Range<int>(cellN,cellN+rangeN));
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
	  Zones.push_back(MapSupport::Range<int>(cNum,dNum));	
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
      
  std::vector<size_t> ZIndex(Zones.size());
  std::iota(ZIndex.begin(),ZIndex.end(),0);
  //  indexSort(Zones,ZIndex);

  return;
}

/// \cond TEMPLATE
  
template class ZoneUnit<double>;
  
/// \endcond TEMPLATE


}  // NAMESPACE physicsSystem
