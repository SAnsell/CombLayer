/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/ObjSurfMap.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <set> 
#include <map> 
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

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
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "surfIndex.h"
#include "surfEqual.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Qhull.h"
#include "ObjSurfMap.h"

#include "debugMethod.h"

namespace ModelSupport
{

void
ObjSurfMap::removeEqualSurf(const std::map<int,Geometry::Surface*>& EQMap,
			    std::map<int,MonteCarlo::Qhull*>& OMap)
  /*!
    Exchanges the surface on the OMap object
    \param EQMap :: Equal map of surface pairs
    \param OMap  :: Map of objects
  */
{
  ELog::RegMethod RegA("ObjSurfMap","removeEqualSurf"); 
  
  ELog::EM<<ELog::endErr;
  std::map<int,MonteCarlo::Qhull*>::iterator mc;
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
  SMap(A.SMap)
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
    }
  return *this;
}

void
ObjSurfMap::clearAll()
  /*!
    Clears all the data
  */
{
  SMap.erase(SMap.begin(),SMap.end());
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
    Get vector of object 
    \param SNum :: Surface number
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
    {
      addSurface(*sc,OPtr);
      /* Remove for simple test [REVSURF]
       const int oSurf= -SurI.hasOpposite(*sc);
       if (oSurf)
       	addSurface(oSurf,OPtr);
      */
    }

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
  // Find is this surface esists
  OMTYPE::iterator mc=SMap.find(SurfN);
  if (mc!=SMap.end())         // surface exists add object
    {
      // Check the object list to see if we have this object
      STYPE& VItem=mc->second;
      STYPE::iterator vc=
	find_if(VItem.begin(),VItem.end(),
		boost::bind(std::equal_to<MonteCarlo::Object*>(),_1,OPtr));
      if (vc==VItem.end())
	mc->second.push_back(OPtr);
    }
  else   // 
    {
      std::pair<OMTYPE::iterator,bool> PIter=
	SMap.insert(OMTYPE::value_type(SurfN,STYPE()));
      PIter.first->second.push_back(OPtr);
    }
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
  STYPE::const_iterator mc;

  for(mc=MVec.begin();mc!=MVec.end();mc++)
    {
      if ((*mc)->getName()!=objExclude && 
	  (*mc)->isDirectionValid(Pos,SN))
	return *mc;
    }
  
  // DEBUG CODE FOR FAILURE:
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  const masterRotate& MR=masterRotate::Instance();

  ELog::EM<<"Failure to find surface on "<<SN<<" "
	  <<MR.calcRotate(Pos)<<" "<<SN<<ELog::endCrit;
  return 0;
  if (SurI.getSurf(abs(SN)))
    ELog::EM<<"Surface == "<<*SurI.getSurf(abs(SN))<<ELog::endWarn;
  else 
    ELog::EM<<"Failed to get == "<<SN<<ELog::endWarn;

  for(mc=MVec.begin();mc!=MVec.end();mc++)
    ELog::EM<<"Common surf Cell  == "<<(*mc)->getName()<<ELog::endDiag;

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
	}
    }
  return;
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
		(boost::bind(&MonteCarlo::Object::getName,_1)));
      OX<<std::endl;
    }
  
}


} // Namespace ModelSupport
