/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedGroup.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedGroup.h"

namespace attachSystem
{

FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const size_t ANL) :
  FixedComp(mainKey,0)
  /*!
    Constructor 
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param ANL :: Size of Fixed group
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(1Group)");

  registerKey(AKey,ANL);
  setDefault(AKey);
}


FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const size_t ANL,
		       const std::string& BKey,
		       const size_t BNL) :
  FixedComp(mainKey,0)
  /*!
    Constructor 
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param ANL :: Size of Fixed group
    \param BKey :: Key Two
    \param BNL :: Size of Fixed group
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(2Group)");

  registerKey(AKey,ANL);
  registerKey(BKey,BNL);

  setDefault(AKey);
  setSecondary(BKey);
}

FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const size_t ANL,
		       const std::string& BKey,
		       const size_t BNL,
		       const std::string& CKey,
		       const size_t CNL) :
  FixedComp(mainKey,0)
  /*!
    Constructor 
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param ANL :: Size of Fixed group
    \param BKey :: Key Two
    \param BNL :: Size of Fixed group
    \param CKey :: Key Three
    \param CNL :: Size of Fixed group
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(3Group)");

  registerKey(AKey,ANL);
  registerKey(BKey,BNL);
  registerKey(CKey,CNL);

  setDefault(AKey);
  setSecondary(BKey);
}

FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const size_t ANL,
		       const std::string& BKey,
		       const size_t BNL,
		       const std::string& CKey,
		       const size_t CNL,
		       const std::string& DKey,
		       const size_t DNL) :
  FixedComp(mainKey,0)
  /*!
    Constructor 
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param ANL :: Size of Fixed group 1
    \param BKey :: Key Two
    \param BNL :: Size of Fixed group 2
    \param CKey :: Key Three
    \param CNL :: Size of Fixed group 3 
    \param DKey :: Key Four
    \param DNL :: Size of Fixed group 4
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(4Group)");

  registerKey(AKey,ANL);
  registerKey(BKey,BNL);
  registerKey(CKey,CNL);
  registerKey(DKey,DNL);

  setDefault(AKey);
  setSecondary(BKey);
}

FixedGroup::FixedGroup(const FixedGroup& A) : 
  FixedComp(A),
  primKey(A.primKey),sndKey(A.sndKey),FMap(A.FMap),
  bX(A.bX),bY(A.bY),bZ(A.bZ),bOrigin(A.bOrigin)
  /*!
    Copy constructor
    \param A :: FixedGroup to copy
  */
{}

FixedGroup&
FixedGroup::operator=(const FixedGroup& A)
  /*!
    Assignment operator
    \param A :: FixedGroup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FixedComp::operator=(A);
      primKey=A.primKey;
      sndKey=A.sndKey;
      FMap=A.FMap;
      bX=A.bX;
      bY=A.bY;
      bZ=A.bZ;
      bOrigin=A.bOrigin;
    }
  return *this;
}


FixedGroup::~FixedGroup()
  /*!
    Deletion operator
  */
{}

void
FixedGroup::registerKey(const std::string& AKey,const size_t NL)
  /*!
    Register a key with objectRegister
    \param AKey :: Map keyname
    \param NL :: Number of links
   */
{
  ELog::RegMethod RegA("FixedGroup","registerKey");

  CompTYPE FCUnit(new FixedUnit(keyName+AKey,NL));  
  FMap.insert(FTYPE::value_type(AKey,FCUnit));
  return;
}

bool
FixedGroup::hasKey(const std::string& Key) const
  /*!
    Check a component to the group
    \param Key :: Key to check
    \return true if key exists
  */
{
  FTYPE::const_iterator mc=FMap.find(Key);
  return (mc!=FMap.end()) ? 1 : 0;
}

FixedComp&
FixedGroup::addKey(const std::string& Key,const size_t NL)
  /*!
    Add a component to the group
    \param Key :: Key to add
    \param NL :: Number of links
    \return Fixed
  */
{
  ELog::RegMethod RegA("FixedGroup","addKey");
  
  FTYPE::iterator mc=FMap.find(Key);
  if (mc!=FMap.end())
    throw ColErr::InContainerError<std::string>
      (Key,"Key present in FMap:"+keyName);

  registerKey(Key,NL);
  mc=FMap.find(Key);
  return *(mc->second);
}
 
FixedComp&
FixedGroup::getKey(const std::string& Key)
  /*!
    Determine the component from the key
    \param Key :: Key to look up
    \return FixedComp 
  */
{
  ELog::RegMethod RegA("FixedGroup","getKey");
  
  FTYPE::iterator mc=FMap.find(Key);
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key in FMap:"+keyName);
  return *(mc->second);
}
 
const FixedComp&
FixedGroup::getKey(const std::string& Key) const
/*!
  Determine the key 
  \param Key :: Key to look up
  \return FixedComp 
*/
{
  ELog::RegMethod RegA("FixedGroup","getKey() const");
  
  FTYPE::const_iterator mc=FMap.find(Key);
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(Key,"Key in FMap:"+keyName);
  return *(mc->second);
}

FixedComp&
FixedGroup::getPrimary() 
  /*!
    Determine the component from the key
    \param Key :: Key to look up
    \return FixedComp 
  */
{
  ELog::RegMethod RegA("FixedGroup","getKey");
  
  FTYPE::iterator mc=FMap.find(primKey);  // can't fail
  return *(mc->second);
}

const FixedComp&
FixedGroup::getPrimary() const
  /*!
    Determine the component from the key
    \param Key :: Key to look up
    \return FixedComp 
  */
{
  ELog::RegMethod RegA("FixedGroup","getKey(const)");
  
  FTYPE::const_iterator mc=FMap.find(primKey);  // can't fail
  return *(mc->second);
}

void
FixedGroup::setDefault(const std::string& defKey)
  /*!
    Sets the default origin/XYZ basis 
    \param defKey :: Keyname to find
   */
{
  ELog::RegMethod RegA("FixedGroup","setDefault");
  
  FTYPE::iterator mc=FMap.find(defKey);
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(defKey,"Key in FMap");

  X=mc->second->getX();
  Y=mc->second->getY();
  Z=mc->second->getZ();
  Origin=mc->second->getCentre();
  primKey=defKey;
  
  return;
  
}

void
FixedGroup::setDefault(const std::string& defKey,
		       const std::string& beamKey)
  /*!
    Sets the default origin/XYZ basis 
    \param defKey :: Keyname to find
    \param beamKey :: Keyname for secondary beam
   */
{
  ELog::RegMethod RegA("FixedGroup","setDefault(key,key)");

  setDefault(defKey);
  setSecondary(beamKey); 

  return;
  
}

void
FixedGroup::setSecondary(const std::string& defKey)
  /*!
    Sets the default origin/XYZ basis 
    \param defKey :: Keyname to find
   */
{
  ELog::RegMethod RegA("FixedGroup","setSecondary");
  
  FTYPE::iterator mc=FMap.find(defKey);
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(defKey,"Key in FMap");

  bX=mc->second->getX();
  bY=mc->second->getY();
  bZ=mc->second->getZ();
  bOrigin=mc->second->getCentre();
  bExit= (mc->second->hasLinkPt(2)) ?
    mc->second->getLinkPt(2) : bOrigin;

  sndKey=defKey;
  return;
  
}

void
FixedGroup::applyRotation(const Geometry::Vec3D& Axis,
			  const double Angle)
  /*!
    Apply a rotation to all groups
f    \param Axis :: rotation axis 
    \param Angle :: rotation angle
  */
{
  ELog::RegMethod RegA("FixedGroup","applyRotation");

  FTYPE::iterator mc;
  for(mc=FMap.begin();mc!=FMap.end();mc++)
    mc->second->applyRotation(Axis,Angle);

  FixedComp::applyRotation(Axis,Angle);
  return;
}
    

void
FixedGroup::applyRotation(const localRotate& LR)
  /*!
    Apply rotation to all groups
    \param LR :: Rotation to apply
   */
{
  ELog::RegMethod RegA("FixedGroup","applyRotation(localRotate)");
  
  
  FTYPE::iterator mc;
  for(mc=FMap.begin();mc!=FMap.end();mc++)
    mc->second->applyRotation(LR);
  FixedComp::applyRotation(LR);
  return;
}
  
void
FixedGroup::setAxisControl(const long int axisIndex,
                           const Geometry::Vec3D& NAxis)
  /*!
    Set the new reorientation axis for a group
    \param axisIndex :: X/Y/Z for reorientation [1-3]
    \param NAxis :: New Axis 
  */
{
  ELog::RegMethod Rega("FixedGroup","setAxisControl");

  for(FTYPE::value_type& MItem : FMap)
    MItem.second->setAxisControl(axisIndex,NAxis);
  return;
}

void
FixedGroup::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    Applies FC[grp](sideIndex) to each component
    which has a name match.
    Then applies FGrp [def](sideIndex) to all 
    remaining units.
    \param FC :: Fixed Component (FixedGroup)
    \param sideIndex :: signed linkpoint			
  */
{
  ELog::RegMethod RegA("FixedGroup","createUnitVector");

  const attachSystem::FixedGroup* FCGrp=
    dynamic_cast<const attachSystem::FixedGroup*>(&FC);
  if (FCGrp)
    {
      // first extract primary group:
      const attachSystem::FixedComp& primFC=FCGrp->getPrimary();
      // key : shared_ptr
      for(FTYPE::value_type& MItem : FMap)
	{
	  FTYPE::const_iterator mc=
	    FCGrp->FMap.find(MItem.first);
	  // if match apply
	  // has key and has correct link points:
	  if (mc!=FCGrp->FMap.end() &&
	      mc->second->NConnect()<= static_cast<size_t>(std::abs(sideIndex)))
	    {
	      MItem.second->createUnitVector(*mc->second,sideIndex);
	    }
	  else  // no match [apply default]
	    MItem.second->createUnitVector(primFC,sideIndex);
	}
    }
  // Only a standard FC unit:
  else
    {
      // key : shared_ptr
      for(FTYPE::value_type& MItem : FMap)
	MItem.second->createUnitVector(FC,sideIndex);
    }
  setDefault(primKey);
  //  FixedComp::createUnitVector();
  return;
}

  

}  // NAMESPACE attachSystem
