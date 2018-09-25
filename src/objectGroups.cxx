/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/objectGroups.cxx
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
#include <memory>
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
#include "surfIndex.h"
#include "surfRegister.h"
#include "groupRange.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "LayerComp.h"

#include "objectGroups.h"

objectGroups::objectGroups() :
  cellZone(10000),cellNumber(1000000)
  /*!
    Constructor
  */
{
  buildMain();
}


objectGroups::objectGroups(const objectGroups& A) : 
  cellZone(A.cellZone),cellNumber(A.cellNumber),
  regionMap(A.regionMap),rangeMap(A.rangeMap),
  Components(A.Components),activeCells(A.activeCells)
  /*!
    Copy constructor
    \param A :: objectGroups to copy
  */
{}

objectGroups&
objectGroups::operator=(const objectGroups& A)
  /*!
    Assignment operator
    \param A :: objectGroups to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellNumber=A.cellNumber;
      regionMap=A.regionMap;
      rangeMap=A.rangeMap;
      Components=A.Components;
      activeCells=A.activeCells;
    }
  return *this;
}
  
objectGroups::~objectGroups() 
  /*!
    Destructor
  */
{}

void
objectGroups::reset()
  /*!
    Delete all the references to the shared_ptr register
  */
{
  Components.erase(Components.begin(),Components.end());
  regionMap.erase(regionMap.begin(),regionMap.end());
  rangeMap.erase(rangeMap.begin(),rangeMap.end());

  activeCells.clear();
  buildMain();
  return;
}

void
objectGroups::buildMain()
  /*!
    Initialize a low cell number for the first 100000 units
  */
{
  regionMap.emplace("Primary",groupRange());
  return;
}

bool
objectGroups::hasCell(const std::string& Name,
		      const int cellN) const
  /*!
    Determine if a cell has been placed in a range
    \param Name :: object name    
    \param cellN :: cell number
    \return true if cellN is registered to region
  */
{
  ELog::RegMethod RegA("objectGroups","hasCell");
  
  MTYPE::const_iterator mc=regionMap.find(Name);
  if (mc==regionMap.end()) return 0;
  const groupRange& GR(mc->second);
  return GR.valid(cellN);
}
  
  
groupRange&
objectGroups::getGroup(const std::string& gName) 
  /*!
    Determine in the cell in range and return object
    thrs can throw if object is not in range
    \param gName :: Group name
    \return groupRange 
   */
{
  ELog::RegMethod RegA("objectGroups","getGroup");
  
  // Quick check to determine if it is the same one as before!
  // Note: groupRange could have been updated to can't store
  MTYPE::iterator mc;
  mc=regionMap.find(gName);

  if (mc==regionMap.end())
    throw ColErr::InContainerError<std::string>(gName,"gName");

  return mc->second;
}

const groupRange&
objectGroups::getGroup(const std::string& gName)  const
  /*!
    Determine in the cell in range and return object
    thrs can throw if object is not in range
    \param gName :: Group name
    \return groupRange 
   */
{
  ELog::RegMethod RegA("objectGroups","getGroup[const]");
  
  // Quick check to determine if it is the same one as before!
  // Note: groupRange could have been updated to can't store
  MTYPE::const_iterator mc;
  mc=regionMap.find(gName);

  if (mc==regionMap.end())
    throw ColErr::InContainerError<std::string>
      (gName,"gName not in groupRange");

  return mc->second;
}

groupRange&
objectGroups::inRangeGroup(const int Index) 
  /*!
    Determine in the cell in range and return object
    thrs can throw if object is not in range
    \param Index :: cell number to get
    \return groupRange 
   */
{
  ELog::RegMethod RegA("objectGroups","inRangeGroup");
  
  static std::string prevName;

  // Quick check to determine if it is the same one as before!
  // Note: groupRange could have been updated to can't store
  MTYPE::iterator mc;
  mc=regionMap.find(prevName);

  if (mc!=regionMap.end() && 
      mc->second.valid(Index))
    return mc->second;

  // ok that failed -- but maybe it is just a zoned name
  const int CNX(Index/cellZone);
  RTYPE::const_iterator rc=rangeMap.find(CNX);
  if (rc!=rangeMap.end())
    {
      mc=regionMap.find(rc->second);
      if (mc!=regionMap.end() && 
	  mc->second.valid(Index))
	{
	  prevName=rc->second;
	  return mc->second;
	}
    }
  
  // Everything failed : exhausive search
  for(mc=regionMap.begin();mc!=regionMap.end();mc++)
    {
      if (mc->second.valid(Index))
	{
	  prevName=mc->first;
	  return mc->second;
	}
    }
  throw ColErr::InContainerError<int>
    (Index,"Cell Index not in groupRange");
}

std::string
objectGroups::inRange(const int Index) const
  /*!
    Determine in the cell in within range
    \param Index :: cell number to test
    \return name of object / empty string if not found
   */
{
  ELog::RegMethod RegA("objectGroups","inRange");
  static std::string prevName;

  // Quick check to determine if it is the same one as before!
  // Note: groupRange could have been updated to can't store
  MTYPE::const_iterator mc;
  mc=regionMap.find(prevName);

  if (mc!=regionMap.end() && 
      mc->second.valid(Index))
    return mc->first;

  // ok that failed -- but maybe it is just a zoned name
  const int CNX(Index/cellZone);
  RTYPE::const_iterator rc=rangeMap.find(CNX);
  if (rc!=rangeMap.end())
    {
      mc=regionMap.find(rc->second);
      if (mc!=regionMap.end() && 
	  mc->second.valid(Index))
	{
	  prevName=rc->second;
	  return prevName;
	}
    }

  for(mc=regionMap.begin();mc!=regionMap.end();mc++)
    {
      if (mc->second.valid(Index))
	{
	  prevName=mc->first;
	  return mc->first;
	}
    }
  return std::string("");
}

std::string
objectGroups::addActiveCell(const int cellN)
  /*!
    Adds an active cell
    \param cellN :: cell number
    \return the activeCell unit
  */
{
  ELog::RegMethod RegA("objectGroups","addActiveCell");
  
  activeCells.insert(cellN);

  std::string unit("Primary");
  if (cellN>10*cellZone)
    {
      const int CNS=cellN/cellZone;
      
      const RTYPE::const_iterator mc=rangeMap.find(CNS);
      if (mc==rangeMap.end())
	throw ColErr::InContainerError<int>
	  (cellN,"CellN/10000 not in rangeMap");
      unit=mc->second;
    }
  
  const MTYPE::iterator mcr=regionMap.find(unit);
  if (mcr==regionMap.end())
    throw ColErr::InContainerError<std::string>
      (unit,"region not in regionMap");
  mcr->second.addItem(cellN);

  return unit;
}

void
objectGroups::removeActiveCell(const int cellN)
  /*!
    Deletes an active cell -- note that this 
    can either (a) be in an normal cell (b) after a renumber
    \param cellN :: cell number
  */
{
  ELog::RegMethod RegA("objectGroups","removeActiveCell");
  
  activeCells.erase(cellN);

  const std::string gName=inRange(cellN);
  if (gName.empty())
    throw ColErr::InContainerError<int>(cellN,"Removing void cell");

  const MTYPE::iterator mcr=regionMap.find(gName);
  if (mcr==regionMap.end())
    throw ColErr::InContainerError<std::string>
      (gName,"region not in regionMap");
  
  mcr->second.removeItem(cellN);
  
  return;
}

void
objectGroups::renumberCell(const int oldCellN,
			   const int newCellN)
  /*!
    Renumber the cell both in the range AND the active
    \param oldCellN :: old cell number
    \param newCellN :: new cell number
  */
{
  ELog::RegMethod RegA("objectGroups","renumberCell");

  // first find if in active unit
  std::set<int>::iterator sc=activeCells.find(oldCellN);
  if (sc==activeCells.end())
    throw ColErr::InContainerError<int>(oldCellN,"Cell number");

  activeCells.erase(sc);
  activeCells.insert(newCellN);

  // Next move the range:
  const std::string gName=inRange(oldCellN);
  if (!gName.empty())
    {
      attachSystem::CellMap* CMPtr=
	getObject<attachSystem::CellMap>(gName);
      if (CMPtr)
	CMPtr->renumberCell(oldCellN,newCellN);

      groupRange& GRP=getGroup(gName);
      GRP.move(oldCellN,newCellN);
    }
  return;
}

  
int
objectGroups::cell(const std::string& Name,const int size)
  /*!
    Add a component and get a new cell number 
    This is called via FixedComp and creates an active range
    \param Name :: Name of the unit
    \param size :: Size of unit to register [units of 10000]
    \return the start number of the cellvalue
  */
{
  ELog::RegMethod RegA("objectGroups","cell");

  ELog::EM<<"ASDFADSF "<<ELog::endDiag;
  if (!size)
    throw ColErr::EmptyValue<size_t>("size");

  const size_t range=( (1+(size-1)/cellZone) );

  MTYPE::const_iterator mc=regionMap.find(Name);
  if (mc!=regionMap.end())
    throw ColErr::InContainerError<std::string>(Name,"Name present");
  
  // create a new region [empty]
  regionMap.emplace(Name,groupRange());
  for(size_t i=0;i<range;i++)
    {
      rangeMap.emplace((cellNumber/cellZone),Name);
      cellNumber+=cellZone;
    }
  
  return cellNumber-cellZone*static_cast<int>(range);
}

void
objectGroups::addObject(const CTYPE& Ptr)
  /*! 
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) No repeat object
    All failures result in an exception.
    \param Ptr :: FixedComp object [shared_ptr]
  */
{
  ELog::RegMethod RegA("objectGroups","addObject(Obj)");
  if (Ptr)
    addObject(Ptr->getKeyName(),Ptr);
  else
    throw ColErr::EmptyValue<void>("Ptr Shared_ptr");
  return;
}

void
objectGroups::addObject(const std::string& Name,
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
  ELog::RegMethod RegA("objectGroups","addObject");
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
objectGroups::isActive(const int cellN) const
  /*!
    Determine if a cell is active
    \param cellN :: Cell number
    \return true if active
  */
{
  return (activeCells.find(cellN)!=activeCells.end());
}

bool
objectGroups::hasObject(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return true (object exists
  */
{
  ELog::RegMethod RegA("objectGroups","hasObject");

  cMapTYPE::const_iterator mc=Components.find(Name);
  return (mc!=Components.end()) ? 1 : 0;
}

attachSystem::FixedComp*
objectGroups::getInternalObject(const std::string& Name) 
  /*!
    Find a FixedComp [if it exists] (from group name if used)
    \param Name :: Name [divided by : FixedGroup : GroupName]
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectGroups","getInternalObject");

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
objectGroups::getInternalObject(const std::string& Name)  const
  /*!
    Find a FixedComp [if it exists] (from group name if used)
    \param Name :: Name [divided by : if group:head]
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectGroups","getInternalObject(const)");

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
objectGroups::getObject(const std::string& Name) const
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectGroups","getObject(const)");

  const attachSystem::FixedComp* FCPtr = getInternalObject(Name);
  return dynamic_cast<const T*>(FCPtr);
}

template<typename T>
T*
objectGroups::getObject(const std::string& Name) 
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectGroups","getObject");
  attachSystem::FixedComp* FCPtr = getInternalObject(Name);
  return dynamic_cast<T*>(FCPtr);
}

template<typename T>
const T*
objectGroups::getObjectThrow(const std::string& Name,
                               const std::string& Err) const
  /*!
    Find a FixedComp [if it exists] 
    Throws InContainerError if not 
    \param Name :: Name
    \param Err :: Error string for exception
    \return ObjectPtr 
  */
{
  ELog::RegMethod RegA("objectGroups","getObjectThrow(const)");
  const T* FCPtr=getObject<T>(Name);
  if (!FCPtr)
    throw ColErr::InContainerError<std::string>(Name,Err);
  return FCPtr;
}

template<typename T>
T*
objectGroups::getObjectThrow(const std::string& Name,
			     const std::string& Err) 
  /*!
    Find a FixedComp [if it exists]
    \param Name :: Name
    \param Err :: Error string for exception
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectGroups","getObjectThrow");
  T* FCPtr=getObject<T>(Name);
  if (!FCPtr)
    throw ColErr::InContainerError<std::string>(Name,Err);
  return FCPtr;
}


template<>
const attachSystem::ContainedComp* 
objectGroups::getObject(const std::string& Name) const
  /*!
    Special for containedComp as it could be a componsite
    of containedGroup
    \param Name :: Name
    \return ObjectPtr / 0 
  */
{
  ELog::RegMethod RegA("objectGroups","getObject(containedComp)");
  
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
objectGroups::getFirstCell(const std::string& objName) const
  /*!
    Returns the first numbe rin a region
    \param objName :: Object name to find
    \return first nubmer [or zero]
  */
{
  MTYPE::const_iterator mc=regionMap.find(objName);
  return (mc==regionMap.end()) ? 0 : mc->second.getFirst();
}

int
objectGroups::getLastCell(const std::string& objName) const
  /*!
    Returns the last number in a region or zero
    \param objName :: Object name to find
    \return region last unit
  */
{
  MTYPE::const_iterator mc=regionMap.find(objName);
  return (mc==regionMap.end()) ? 0 : mc->second.getLast();
}

std::vector<int>
objectGroups::getObjectRange(const std::string& objName) const
  /*!
    Calculate the object cells range based on the name
    Processes down to cellMap items if objName is of the 
    form objecName:cellMapName
    \param objName :: Object name
    \return vector of item
  */
{
  ELog::RegMethod RegA("objectGroups","getObjectRange");
  
  std::string::size_type pos=objName.find(":");
  // CELLMAP Range ::  objectName:cellName
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
      return Out;
    }
  
  // SIMPLE NUMBER RANGE  M - N
  pos=objName.find("-");
  if (pos!=std::string::npos)
    {
      int ANum,BNum;
      const std::string AName=objName.substr(0,pos);
      const std::string BName=objName.substr(pos+1);
      if (!StrFunc::convert(AName,ANum) ||
          !StrFunc::convert(BName,BNum) )
        throw ColErr::InContainerError<std::string>
          (objName,"objectName does not convert to numbers");
      
      if (ANum>BNum)
        std::swap(ANum,BNum);
      std::vector<int> Out;
      for(int index=ANum;index<BNum;index++)
	if (isActive(index))
	  Out.push_back(index);

      return Out;
    }

  // SPECIALS:
  if (objName=="All" || objName=="all")
    return std::vector<int>(activeCells.begin(),activeCells.end());

  // BASIC
  const groupRange& GRP=getGroup(objName); 
  return GRP.getAllCells();
}
  
void
objectGroups::rotateMaster()
  /*!
    Apply the rotation to the object component
   */
{
  ELog::RegMethod RegA("objectGroups","rotateMaster");
  const masterRotate& MR=masterRotate::Instance();
  
  for(cMapTYPE::value_type& AUnit : Components)
    AUnit.second->applyRotation(MR);
  
  return;
}

  
void
objectGroups::write(const std::string& OFile) const
  /*!
    Write out to a file 
    \param OFile :: output file
  */
{
  ELog::RegMethod RegA("objectGroups","write");
  if (!OFile.empty())
    {
      const char* FStatus[]={"void","fixed"};
      std::ofstream OX(OFile.c_str());

      boost::format FMT("%s%|40t|(%s)");
      MTYPE::const_iterator mc;
      for(mc=regionMap.begin();mc!=regionMap.end();mc++)
	{
	  const CTYPE::element_type* FPTR=
	    getObject<CTYPE::element_type>(mc->first);
	  const int flag=(FPTR) ? 1 : 0;
	  OX<<(FMT % mc->first) % FStatus[flag];
	  if (flag)
	    OX<<" "<<FPTR->getCentre();
	  OX<<" :: "<<mc->second<<std::endl;
	}
    }
  return;
}

///\cond TEMPLATE
  
template const attachSystem::FixedComp* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::ContainedComp* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::ContainedGroup* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::TwinComp* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::SecondTrack* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::LayerComp* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::CellMap* 
  objectGroups::getObject(const std::string&) const;

template const attachSystem::SurfMap* 
  objectGroups::getObject(const std::string&) const;

template attachSystem::FixedComp* 
  objectGroups::getObject(const std::string&);

template attachSystem::FixedGroup* 
  objectGroups::getObject(const std::string&);

template attachSystem::ContainedComp* 
  objectGroups::getObject(const std::string&);

template attachSystem::ContainedGroup* 
  objectGroups::getObject(const std::string&);

template attachSystem::TwinComp* 
  objectGroups::getObject(const std::string&);

template attachSystem::SecondTrack* 
  objectGroups::getObject(const std::string&);

template attachSystem::CellMap* 
  objectGroups::getObject(const std::string&);

template attachSystem::SurfMap* 
  objectGroups::getObject(const std::string&);



template const attachSystem::FixedComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::ContainedComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::ContainedGroup* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::TwinComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::SecondTrack* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::LayerComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::CellMap* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template const attachSystem::SurfMap* 
  objectGroups::getObjectThrow(const std::string&,const std::string&) const;

template attachSystem::FixedComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::FixedGroup* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::ContainedComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::ContainedGroup* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::TwinComp* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::SecondTrack* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::CellMap* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);

template attachSystem::SurfMap* 
  objectGroups::getObjectThrow(const std::string&,const std::string&);


  
///\endcond TEMPLATE  

