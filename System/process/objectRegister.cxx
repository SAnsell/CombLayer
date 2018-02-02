/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/objectRegister.cxx
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
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "support.h"
#include "stringCombine.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "LayerComp.h"
#include "objectRegister.h"

namespace ModelSupport
{

objectRegister::objectRegister() : 
  cellNumber(1000000)
  /*!
    Constructor
  */
{}

objectRegister::~objectRegister() 
  /*!
    Destructor
  */
{}

void
objectRegister::reset()
  /*!
    Delete all the references to the shared_ptr register
  */
{
  Components.erase(Components.begin(),Components.end());
  activeCells.clear();
  return;
}
  
objectRegister& 
objectRegister::Instance() 
  /*!
    Effective this object			
    \return objectRegister object
  */
{
  static objectRegister A;
  return A;
}

int
objectRegister::getCell(const std::string& Name) const
  /*!
    Get the start cell of an object
    \param Name :: Name of the object to get
    \return Cell number of first cell in range
  */
{
  MTYPE::const_iterator mc;
  mc=regionMap.find(Name);
  return (mc!=regionMap.end())
    ? mc->second.first : 0;
}

int
objectRegister::getRange(const std::string& Name) const
  /*!
    Get the range of an object
    \param Name :: Name of the object to get
    \return Range
   */
{
  MTYPE::const_iterator mc=
    regionMap.find(Name);

  return (mc!=regionMap.end()) ?
    (mc->second.second-mc->second.first) : 0;
}

int
objectRegister::getLast(const std::string& Name) const
  /*!
    Get the last cell in the range of an object
    \param Name :: Name of the object to get
    \return Range
   */
{
  MTYPE::const_iterator mc=
    regionMap.find(Name);

  return (mc!=regionMap.end()) ?
    mc->second.second : 0;
}

bool
objectRegister::hasCell(const std::string& Name,
			const int cellN) const
  /*!
    Determine if a cell is within a range
    \param Name :: object name    
    \param cellN :: cell number
    \return true if cellN is registered to region
  */
{
  ELog::RegMethod RegA("objectRegister","hasCell");
  MTYPE::const_iterator mc=regionMap.find(Name);
  if (mc==regionMap.end()) return 0;
  const MTYPE::mapped_type& PI(mc->second);
  return (cellN>=PI.first && cellN<=PI.second) ? 1 : 0;
}
  
  
std::string
objectRegister::inRange(const int Index) const
  /*!
    Determine in the cell in within range
    \param Index :: cell number to test
    \return string
   */
{
  static std::string prev;
  
  MTYPE::const_iterator mc;
  mc=regionMap.find(prev);

  if (mc!=regionMap.end() && 
      Index>=mc->second.first && 
      Index<=mc->second.second)
    return mc->first;
    
  for(mc=regionMap.begin();mc!=regionMap.end();mc++)
    {
      const std::pair<int,int>& IP=mc->second;
      if (Index>=IP.first && Index<=IP.second)
	{
	  prev=mc->first;
	  return mc->first;
	}
    }
  return std::string("");
}

void
objectRegister::addActiveCell(const int cellN)
  /*!
    Adds an active cell
    \param cellN :: cell number
  */
{
  activeCells.insert(cellN);
  return;
}

void
objectRegister::removeActiveCell(const int cellN)
  /*!
    Deletes an active cell
    \param cellN :: cell number
  */
{
  activeCells.erase(cellN);
  return;
}

void
objectRegister::renumberActiveCell(const int oldCellN,
                                   const int newCellN)
  /*!
    Renumber the active set
    \param oldCellN :: old cell number
    \param newCellN :: new cell number
  */
{
  ELog::RegMethod RegA("objectRegister","renumberActive");

  std::set<int>::iterator sc=activeCells.find(oldCellN);
  if (sc==activeCells.end())
    throw ColErr::InContainerError<int>(oldCellN,"Cell number");

  activeCells.erase(sc);
  activeCells.insert(newCellN);

  return;
}

  
int
objectRegister::cell(const std::string& Name,const int size)
  /*!
    Add a component and get a new cell number 
    \param Name :: Name of the unit
    \param size :: Size of unit to register
    \return the start number of the cellvalue
  */
{
  ELog::RegMethod RegA("objectRegister","cell");


  MTYPE::const_iterator mc=regionMap.find(Name);  
  if (mc!=regionMap.end())
    {
      if (mc->second.second<size)
	ELog::EM<<"Insufficient space reserved for "<<Name<<ELog::endErr;
      return mc->second.first;
    }
  regionMap.emplace(Name,std::pair<int,int>(cellNumber,cellNumber+size));
  cellNumber+=size;
  return cellNumber-size;
}

void
objectRegister::addObject(const CTYPE& Ptr)
  /*! 
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) No repeat object
    All failures result in an exception.
    \param Ptr :: FixedComp object [shared_ptr]
  */
{
  ELog::RegMethod RegA("objectRegister","addObject(Obj)");
  if (Ptr)
    addObject(Ptr->getKeyName(),Ptr);
  else
    throw ColErr::EmptyValue<void>("Ptr Shared_ptr");
  return;
}

void
objectRegister::addObject(const std::string& Name,
			 const CTYPE& Ptr)
  /*!
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) No repeat object
    All failures result in an exception.
    \param Name :: Name of the object						
    \param Ptr :: Shared_ptr
  */
{
  ELog::RegMethod RegA("objectRegister","addObject");
  // First check that we have it in Register:
  if (regionMap.find(Name)==regionMap.end())
    throw ColErr::InContainerError<std::string>(Name,"regionMap empty");
  // Does it exist:
  if (Components.find(Name)!=Components.end())
    throw ColErr::InContainerError<std::string>(Name,"Exisiting object");
  Components.insert(cMapTYPE::value_type(Name,Ptr));
  return;
}

bool
objectRegister::hasObject(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return true (object exists
  */
{
  ELog::RegMethod RegA("objectRegister","hasObject");

  cMapTYPE::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? 1 : 0;
}

attachSystem::FixedComp*
objectRegister::getInternalObject(const std::string& Name) 
  /*!
    Find a FixedComp [if it exists] (from group name if used)
    \param Name :: Name [divided by : if group:head]
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getInternalObject");

  const std::string::size_type pos=Name.find(":");

  if (pos!=0 && pos!=std::string::npos)
    {
      const std::string head=Name.substr(0,pos);
      const std::string tail=Name.substr(pos+1);
      cMapTYPE::iterator mcx=Components.find(head);
      if (mcx!=Components.end())
	{
	  attachSystem::FixedGroup* FGPtr=
	    dynamic_cast<attachSystem::FixedGroup*>(mcx->second.get());
	  return (FGPtr && FGPtr->hasKey(tail)) ?
            &(FGPtr->getKey(tail)) : 0;
	}
      // Fall through here to test whole name:
    }
  
  cMapTYPE::iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? mc->second.get() : 0;
}

const attachSystem::FixedComp*
objectRegister::getInternalObject(const std::string& Name)  const
  /*!
    Find a FixedComp [if it exists] (from group name if used)
    \param Name :: Name [divided by : if group:head]
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getInternalObject(const)");

  const std::string::size_type pos=Name.find(":");

  if (pos!=0 && pos!=std::string::npos)
    {
      const std::string head=Name.substr(0,pos);
      const std::string tail=Name.substr(pos+1);
      cMapTYPE::const_iterator mcx=Components.find(head);
      if (mcx!=Components.end())
	{
	  const attachSystem::FixedGroup* FGPtr=
	    dynamic_cast<const attachSystem::FixedGroup*>(mcx->second.get());
	  if (FGPtr)
	    return (FGPtr->hasKey(tail)) ? &(FGPtr->getKey(tail)) : 0;
	}
      // Fall through here to test whole name:
    }
  
  cMapTYPE::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? mc->second.get() : 0;
}

template<typename T>
const T*
objectRegister::getObject(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObject(const)");

  const attachSystem::FixedComp* FCPtr = getInternalObject(Name);
  return dynamic_cast<const T*>(FCPtr);
}

template<typename T>
T*
objectRegister::getObject(const std::string& Name) 
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObject");
  attachSystem::FixedComp* FCPtr = getInternalObject(Name);
  return dynamic_cast<T*>(FCPtr);
}

template<typename T>
const T*
objectRegister::getObjectThrow(const std::string& Name,
                               const std::string& Err) const
  /*!
    Find a FixedComp [if it exists] 
    Throws InContainerError if not 
    \param Name :: Name
    \param Err :: Error string for exception
    \return ObjectPtr 
  */
{
  ELog::RegMethod RegA("objectRegister","getObjectThrow(const)");
  const T* FCPtr=getObject<T>(Name);
  if (!FCPtr)
    throw ColErr::InContainerError<std::string>(Name,Err);
  return FCPtr;
}

template<typename T>
T*
objectRegister::getObjectThrow(const std::string& Name,
                                const std::string& Err) 
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \param Err :: Error string for exception
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObjectThrow");
  T* FCPtr=getObject<T>(Name);
  if (!FCPtr)
    throw ColErr::InContainerError<std::string>(Name,Err);
  return FCPtr;
}


template<>
const attachSystem::ContainedComp* 
objectRegister::getObject(const std::string& Name) const
  /*!
    Special for containedComp as it could be a componsite
    of containedGroup
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectRegister","getObject(containedComp)");
  
  const std::string::size_type pos=Name.find(":");
  if (pos==std::string::npos || !pos || pos==Name.size()-1)
    {
      cMapTYPE::const_iterator mc=Components.find(Name);
      return (mc!=Components.end()) ?
	dynamic_cast<const attachSystem::ContainedComp*>(mc->second.get()) 
	: 0;
    }
  const std::string PreItem=Name.substr(0,pos);
  const std::string PostItem=Name.substr(pos);
  return 0;
}

int
objectRegister::getRenumberCell(const std::string& Name) const
  /*!
    Get the start cell of an object [renumbered]
    \param Name :: Name of the object to get
    \return Cell number
  */
{
  MTYPE::const_iterator mc;
  mc=renumMap.find(Name);
  
  if (mc!=renumMap.end())
    return mc->second.first;
  // maybe we have object but it is actually zero celled
  mc=regionMap.find(Name);
  return (mc!=regionMap.end()) ? mc->second.first : 0;
}

int
objectRegister::getRenumberRange(const std::string& Name) const
  /*!
    Get the range of cells of an object
    \param Name :: Name of the object to get
    \return Cell number
  */
{
  // NOTE: renumber not always complete as zero cell objects not present
  MTYPE::const_iterator mc=
    renumMap.find(Name);
  
  if (mc!=renumMap.end())
    return mc->second.second;
  // maybe we have object but it is actually zero celled
  mc=regionMap.find(Name);
  return (mc!=regionMap.end()) ? mc->second.second : 0;
}

std::string
objectRegister::inRenumberRange(const int Index) const
  /*!
    Get the range of an object
    \param Index :: Offset number
    \return string of object
   */
{
  static std::string prev;
  
  MTYPE::const_iterator mc;
  // normally same as previous search
  if (!prev.empty())
    {
      mc=renumMap.find(prev);
      if (mc!=renumMap.end() && 
	  Index>=mc->second.first && 
	  Index<=mc->second.second)
	return mc->first;
    }
  for(mc=renumMap.begin();mc!=renumMap.end();mc++)
    {
      const std::pair<int,int>& IP=mc->second;
      if (Index>=IP.first && Index<=IP.second)
	{
	  prev=mc->first;
	  return mc->first;
	}
    }
  return std::string("");
}

  
void
objectRegister::setRenumber(const std::string& key,
			    const int startN,const int endN)
  /*!
    Insert renumber into the system :
    \param key :: Keyname [nop if not present in renumMap]
    \param startN :: First cell number
    \param endN :: last cell number
  */
{
  ELog::RegMethod RegA("objectRegister","setRenumber");
  if (regionMap.find(key)!=regionMap.end())
    {
      MTYPE::iterator mc=renumMap.find(key);
      if (mc!=renumMap.end())
	mc->second=std::pair<int,int>(startN,endN);
      else
	renumMap.emplace(key,std::pair<int,int>(startN,endN));
    }
  return;
}
  
int
objectRegister::calcRenumber(const int CN) const
  /*!
    Take a cell number and calculate the renumber [not ideal]
    \param CN :: orignal cell number
    \return correct offset number
   */
{
  const std::string key=inRange(CN);
  if (key.empty())
    return CN;

  MTYPE::const_iterator Amc=regionMap.find(key);
  MTYPE::const_iterator Bmc=renumMap.find(key);
  if (Bmc==renumMap.end())
    return CN;

  const int Cdiff=CN-Amc->second.first;
  return Bmc->second.first+Cdiff-1;
}

std::vector<int>
objectRegister::getObjectRange(const std::string& objName) const
  /*!
    Calculate the object cells range based on the name
    Processes down to cellMap items if objName is of the 
    form objecName:cellMapName
    \param objName :: Object name
    \return vector of item
  */
{
  ELog::RegMethod RegA("objectRegister","getObjectRange");

  std::string::size_type pos=objName.find(":");
  // CELL Range ::  objectName:cellName
  if (pos!=0 && pos!=std::string::npos)
    {
      const std::string itemName=objName.substr(0,pos);
      const std::string cellName=objName.substr(pos+1);

      const attachSystem::CellMap* CPtr=
        getObject<attachSystem::CellMap>(itemName);
      if (!CPtr)
        throw ColErr::InContainerError<std::string>(itemName,"objectName:");
      
      std::vector<int> Out=CPtr->getCells(cellName);
      if (Out.empty())
        {
          ELog::EM<<"EMPTY NAME::Possible names["<<itemName
		  <<"] == "<<ELog::endDiag;
          std::vector<std::string> NameVec=CPtr->getNames();
          for(const std::string CName : NameVec)
            ELog::EM<<"  "<<CName<<ELog::endDiag;
          throw ColErr::InContainerError<std::string>
            (objName,"Object empty");
        }
      
      for(int& CN : Out)
	CN=calcRenumber(CN);
      return Out;
    }
  
  // Simple number range
  pos=objName.find("-");
  if (pos!=std::string::npos)
    {
      long int ANum,BNum;
      const std::string AName=objName.substr(0,pos);
      const std::string BName=objName.substr(pos+1);
      if (!StrFunc::convert(AName,ANum) ||
          !StrFunc::convert(BName,BNum) )
        throw ColErr::InContainerError<std::string>
          (objName,"objectName does not convert to numbers");
      if (ANum>BNum)
        std::swap(ANum,BNum);
      std::vector<int> Out(static_cast<size_t>(1+BNum-ANum));
      std::iota(Out.begin(),Out.end(),ANum);
      for(int& CN : Out)
        CN=calcRenumber(CN);

      return Out;
    }

  // SPECIALS:
  if (objName=="All" || objName=="all")
    {
      std::vector<int> Out;
      for(const int CN : activeCells)
        Out.push_back(calcRenumber(CN));
      return Out;
    }

  
  // Just an object name:

  const int BStart=getCell(objName);
  const int BRange=getRange(objName);

  if (BStart==0)
    throw ColErr::InContainerError<std::string>
      (objName,"Object name not found");
  
  if (!BRange)
    return std::vector<int>();
  // Loop forward to find first element in set :
  // then step forward until out of range.
  std::vector<int> Out;
  std::set<int>::const_iterator sc=activeCells.end();

  for(int i=BStart;i<BRange+BStart;i++)
    {
      sc=activeCells.find(i);
      if (sc!=activeCells.end())
	Out.push_back(*sc);
      
    }
  for(int& CN : Out)
    CN=calcRenumber(CN);
  return Out;
}
  
void
objectRegister::rotateMaster()
  /*!
    Apply the rotation to the object component
   */
{
  ELog::RegMethod RegA("objectRegister","rotateMaster");
  const masterRotate& MR=masterRotate::Instance();
  
  for(cMapTYPE::value_type& AUnit : Components)
    AUnit.second->applyRotation(MR);

  return;
}

  
void
objectRegister::write(const std::string& OFile) const
  /*!
    Write out to a file
    \param OFile :: output file
  */
{
  ELog::RegMethod RegA("objectRegister","write");
  if (!OFile.empty())
    {
      const char* FStatus[]={"void","fixed"};
      std::ofstream OX(OFile.c_str());

      boost::format FMT("%s%|30t|%d    ::     %d %|20t|(%s)");
      MTYPE::const_iterator mc;
      for(mc=regionMap.begin();mc!=regionMap.end();mc++)
	{
	  const CTYPE::element_type* FPTR=
	    getObject<CTYPE::element_type>(mc->first);
	  const int flag=(FPTR) ? 1 : 0;
	  OX<<(FMT % mc->first % mc->second.first % 
	       mc->second.second % FStatus[flag]);
	  if (flag)
	    OX<<" "<<FPTR->getCentre();
	  OX<<std::endl;

	}
    }
  return;
}

///\cond TEMPLATE
  
template const attachSystem::FixedComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::ContainedComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::ContainedGroup* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::TwinComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::SecondTrack* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::LayerComp* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::CellMap* 
  objectRegister::getObject(const std::string&) const;

template const attachSystem::SurfMap* 
  objectRegister::getObject(const std::string&) const;

template attachSystem::FixedComp* 
  objectRegister::getObject(const std::string&);

template attachSystem::FixedGroup* 
  objectRegister::getObject(const std::string&);

template attachSystem::ContainedComp* 
  objectRegister::getObject(const std::string&);

template attachSystem::ContainedGroup* 
  objectRegister::getObject(const std::string&);

template attachSystem::TwinComp* 
  objectRegister::getObject(const std::string&);

template attachSystem::SecondTrack* 
  objectRegister::getObject(const std::string&);

template attachSystem::CellMap* 
  objectRegister::getObject(const std::string&);

template attachSystem::SurfMap* 
  objectRegister::getObject(const std::string&);



template const attachSystem::FixedComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::ContainedComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::ContainedGroup* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::TwinComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::SecondTrack* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::LayerComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::CellMap* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::SurfMap* 
  objectRegister::getObjectThrow(const std::string&,const std::string&) const;

template attachSystem::FixedComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::FixedGroup* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::ContainedComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::ContainedGroup* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::TwinComp* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::SecondTrack* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::CellMap* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);

template attachSystem::SurfMap* 
  objectRegister::getObjectThrow(const std::string&,const std::string&);


  
///\endcond TEMPLATE  

} // NAMESPACE ModelSupport
