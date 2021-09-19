/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ObjSurfMap.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <set> 
#include <map> 
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "Surface.h"
#include "surfIndex.h"
#include "ObjSurfMap.h"


namespace ModelSupport
{

void
ObjSurfMap::removeEqualSurf(const std::map<int,Geometry::Surface*>& EQMap,
			    std::map<int,MonteCarlo::Object*>& OMap)
  /*!
    Exchanges the surface on the OMap object
    \param EQMap :: Equal map of surface pairs
    \param OMap  :: Map of objects
  */
{
  ELog::RegMethod RegA("ObjSurfMap","removeEqualSurf"); 
  
  ELog::EM<<ELog::endErr;
  std::map<int,MonteCarlo::Object*>::iterator mc;
  std::map<int,Geometry::Surface*>::const_iterator ec;
  for(mc=OMap.begin();mc!=OMap.end();mc++)
    {
      for(ec=EQMap.begin();ec!=EQMap.end();ec++)
	mc->second->substituteSurf(ec->first,
				   ec->second->getName(),ec->second);
    }
  return;
}

ObjSurfMap::ObjSurfMap() 
 /*! 
   Constructor 
 */
{}

ObjSurfMap::ObjSurfMap(const ObjSurfMap& A) :
  SMap(A.SMap),OSurfMap(A.OSurfMap)
  /*! 
    Copy Constructor 
    \param A :: ObjSurfMap to copy
  */
{}

ObjSurfMap&
ObjSurfMap::operator=(const ObjSurfMap& A) 
  /*! 
    Assignment operator
    \param A :: ObjSurfMap to copy
    \return *this
  */
{
  if (this!=&A)
    {
      SMap=A.SMap;
      OSurfMap=A.OSurfMap;
    }
  return *this;
}

void
ObjSurfMap::clearAll()
  /*!
    Clears all the data
  */
{
  SMap.clear();
  OSurfMap.clear();
  return;
}

MonteCarlo::Object* 
ObjSurfMap::getObj(const int SNum,const size_t Index) const
  /*!
    Search for a signed surface number
    \param SNum :: Surface number 
    \param Index :: Index number [of object in list]
    \return Object Ptr if surface with appropiate sign exists
   */
{
  OMTYPE::const_iterator mc=SMap.find(SNum);
  if (mc==SMap.end()) return 0;
  
  return (Index < mc->second.size())  
	  ? mc->second[Index] : 0;
}

const ObjSurfMap::STYPE&
ObjSurfMap::getObjects(const int SNum) const
  /*!
    Get vector of objects which have a surface SNum.
    \param SNum :: Surface number [signed]
    \return empty set if SNum doesn't match
   */
{
  static STYPE emptyVec;

  ELog::RegMethod RegA("ObjSurfMap","getObjects");

  OMTYPE::const_iterator mc=SMap.find(SNum);
  return (mc==SMap.end()) ? emptyVec : mc->second;
}

void
ObjSurfMap::addSurfaces(MonteCarlo::Object* OPtr)
  /*!
    Adds all the surface from an object
    \param OPtr :: Object pointer
   */
{
  ELog::RegMethod RegA("ObjSurfMap","addSurfaces");
  
  // signed set
  const std::set<int>& sSet=OPtr->getSurfSet();

  std::set<int>::const_iterator sc;
  for(sc=sSet.begin();sc!=sSet.end();sc++)
    addSurface(*sc,OPtr);

  return;
}

void
ObjSurfMap::updateObject(MonteCarlo::Object* OPtr)
  /*!
    Updates an existing object
    \param OPtr :: Object pointer
   */
{
  ELog::RegMethod RegA("ObjSurfMap","updateObject");

  // first do we have the object?
  const int objName(OPtr->getName());
  OSTYPE::iterator oc=OSurfMap.find(objName);
  if (oc!=OSurfMap.end())
    {
      // must remove all valid surfaces first as rev surfaces
      // make a mess
      const std::set<int>& sSet=oc->second;
      for(const int SN : sSet)
	{
	  OMTYPE::iterator mc=SMap.find(SN);
	  if (mc!=SMap.end())
	    {
	      STYPE& vec=mc->second;
	      STYPE::iterator vc=std::find(vec.begin(),vec.end(),OPtr);
	      if (vc!=vec.end())
		vec.erase(vc);
	      if (vec.empty())
		SMap.erase(mc);
	    }
	}
      OSurfMap.erase(oc);
      
      // add object as ifnew :
      addSurfaces(OPtr);
      return;
    }

  return;
}

void
ObjSurfMap::addObjectSurf(const MonteCarlo::Object* OPtr,
                          const int SurfN)
  /*!
    Adds a object to the object set and a considered surface 
    number.
    \param OPtr :: Object Ptr
    \param SurfN :: Surface number
  */
{
  ELog::RegMethod RegA("ObjSurfMap","addObjectSurf");

  const int ON=OPtr->getName();
  OSTYPE::iterator mc=OSurfMap.find(ON);
  if (mc!=OSurfMap.end())
    mc->second.insert(SurfN);
  else
    OSurfMap.emplace(OSTYPE::value_type(ON,surfTYPE({SurfN})));

  return;
}

void
ObjSurfMap::removeObjectSurface(const int SurfN)
  /*!
    Remove the surface from all objects
    \param SurfN :: Surface number to remove
  */
{
  ELog::RegMethod RegA("ObjSurfMap","removeObjectSurface");

  for(OSTYPE::value_type& mc : OSurfMap)
    mc.second.erase(SurfN);
  
  return;
}
  
void
ObjSurfMap::addSurface(const int SurfN,MonteCarlo::Object* OPtr)
  /*!
    Adds all the surface from an specific surface on an object
    \param SurfN :: Surface number to add
    \param OPtr :: Object pointer
   */
{
  ELog::RegMethod RegA("ObjSurfMap","addSurface");

  // Find is this surface exists
  OMTYPE::iterator mc=SMap.find(SurfN);
  if (mc!=SMap.end())         // surface exists add object
    {
      // Check the object list to see if we have this object
      STYPE& VItem=mc->second;
      STYPE::iterator vc=
	find_if(VItem.begin(),VItem.end(),
		std::bind(std::equal_to<MonteCarlo::Object*>(),
			  std::placeholders::_1,OPtr));
      if (vc==VItem.end())
	mc->second.push_back(OPtr);
    }
  else   
    {
      std::pair<OMTYPE::iterator,bool> PIter=
	SMap.insert(OMTYPE::value_type(SurfN,STYPE()));
      PIter.first->second.push_back(OPtr);
    }
  addObjectSurf(OPtr,SurfN);
  return;
}

MonteCarlo::Object*
ObjSurfMap::findNextObject(const int SN,
			   const Geometry::Vec3D& Pos,
			   const int objExclude) const
  /*!
    Calculate the next object 
    \param SN :: Surface number
    \param Pos :: position
    \param ObjExclude :: Excluded object
    \return Next Object Ptr / 0 on point not valid
  */
{
  ELog::RegMethod RegA("ObjSurfMap","findNextObject");

  const STYPE& MVec=getObjects(SN);

  for(MonteCarlo::Object* MPtr : MVec)
    {
      if (MPtr->getName()!=objExclude && 
	  MPtr->isDirectionValid(Pos,SN))
	return MPtr;
    }
  return 0;
}

void
ObjSurfMap::removeReverseSurf(const int primSurf,const int revSurf)
  /*!
    Find those surfaces which are listed as rev and exchange them
    for those that are listed as seconadary
    \param primSurf :: Kept surface [added in case of reverse surf]
    \param revSurf :: Surf that is removed
  */
{
  ELog::RegMethod RegA("ObjSurfMap","removeReverseSurf");
  
  // Remove reverse surface from maps: [+ve]
  
  for(int sign_index=-1;sign_index<2;sign_index+=2)
    {
      OMTYPE::iterator ac=SMap.find(sign_index*revSurf);
      if (ac!=SMap.end())
	{
	  STYPE& OVec=ac->second;
	  STYPE::iterator oc;
	  // add to reverse list of opposite signed prim surf
	  for(oc=OVec.begin();oc!=OVec.end();oc++)
	    addSurface(-sign_index*primSurf,*oc);
	  // Now remove item list
	  SMap.erase(ac);
          removeObjectSurface(sign_index*revSurf);
	}
    }
  return;
}


void
ObjSurfMap::removeObject(const MonteCarlo::Object* OPtr)
  /*!
    Remove a specific object.
    Expensive call to remove all instances of the cell nubmer
    from the OSM.
    \param cellNumber :: Cell to remove
  */
{
  ELog::RegMethod Rega("ObjSurfMap","removeObject");

  const int cellNumber(OPtr->getName());
  
  OSTYPE::iterator mc=OSurfMap.find(cellNumber);
  if (mc!=OSurfMap.end())
    {
      const std::set<int>& surfIndex=mc->second;
      for(const int SN : surfIndex)
	{
	  OMTYPE::iterator sm=SMap.find(SN);
	  if (sm!=SMap.end())
	    {
	      STYPE& ObjVec=sm->second;
	      STYPE::iterator objITER=find(ObjVec.begin(),ObjVec.end(),OPtr);
	      if (objITER!=ObjVec.end())
		ObjVec.erase(objITER);
	      if (ObjVec.empty())
		SMap.erase(sm);
	    }
	}
      OSurfMap.erase(mc);
    }
  return;
}
  
const std::set<int>&
ObjSurfMap::connectedObjects(const int cellNumber) const
  /*!
    Given an object, find all the objects that can connect
    to it. 
    \param cellNumber :: Cell number
    \return set of object names
  */
{
  ELog::RegMethod RegA("ObjSurfMap","connectedObject");
  static std::set<int> Empty;  // Empty set to return
  

  OSTYPE::const_iterator mc=OSurfMap.find(cellNumber);
  return (mc!=OSurfMap.end()) ? mc->second : Empty;

}

bool
ObjSurfMap::isConnected(const int cellA,const int cellB) const
  /*!
    Determine if two objects are connected
    \param cellA :: Cell number
    \param cellB :: Cell number
    \return true if connected
  */
{
  ELog::RegMethod RegA("ObjSurfMap","isConnected");

  OSTYPE::const_iterator mAc=OSurfMap.find(cellA);
  OSTYPE::const_iterator mBc=OSurfMap.find(cellB);
  if (mAc==OSurfMap.end() || mBc==OSurfMap.end())
    return 0;

  const std::set<int>& aSet=mAc->second;
  const std::set<int>& bSet=mBc->second;

  for(const int SN : aSet)
    {
      if (bSet.find(-SN)!=bSet.end()) return 1;
    }
  
  return 0;
}

void
ObjSurfMap::write(const std::string& FName) const
  /*!
    Write out the map to a file
    \param FName :: File
  */
{
  if (!FName.empty())
    {
      std::ofstream FX(FName.c_str());
      write(FX);
    }
  return;
}

void
ObjSurfMap::write(std::ostream& OX) const
  /*!
    Write to a standard stream
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("ObjSurfMap","write");

  OX<<"# Object Map"<<std::endl;
  OMTYPE::const_iterator mc;

  for(mc=SMap.begin();mc!=SMap.end();mc++)
    {
      OX<<mc->first<<":";

      transform(mc->second.begin(),mc->second.end(),
		std::ostream_iterator<int>(OX," "),
		(std::bind(&MonteCarlo::Object::getName,
			   std::placeholders::_1)));
      OX<<std::endl;
    }
  
}


} // Namespace ModelSupport
