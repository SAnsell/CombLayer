/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedGroup.cxx
 *
 * Copyright (c) 2004-2026 by Stuart Ansell
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
		       const std::string& AKey) :
  FixedComp(mainKey)
  /*!
    Constructor
    \param mainKey :: mainKey
    \param AKey :: Key one
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(1Group)");

  registerKey(AKey);
  setDefault(AKey);
}


FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const std::string& BKey) :
  FixedComp(mainKey)
  /*!
    Constructor
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param BKey :: Key Two
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(2Group)");

  registerKey(AKey);
  registerKey(BKey);

  setDefault(AKey);
  setSecondary(BKey);
}

FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const std::string& BKey,
		       const std::string& CKey) :
  FixedComp(mainKey)
  /*!
    Constructor
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param BKey :: Key Two
    \param CKey :: Key Three
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(3Group)");

  registerKey(AKey);
  registerKey(BKey);
  registerKey(CKey);

  setDefault(AKey);
  setSecondary(BKey);
}

FixedGroup::FixedGroup(const std::string& mainKey,
		       const std::string& AKey,
		       const std::string& BKey,
		       const std::string& CKey,
		       const std::string& DKey) :
  FixedComp(mainKey)
  /*!
    Constructor
    \param mainKey :: mainKey
    \param AKey :: Key one
    \param BKey :: Key Two
    \param CKey :: Key Three
    \param DKey :: Key Four
  */
{
  ELog::RegMethod RegA("FixedGroup","constructor(4Group)");

  registerKey(AKey);
  registerKey(BKey);
  registerKey(CKey);
  registerKey(DKey);

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
FixedGroup::registerKey(const std::string& AKey)
  /*!
    Register a key with objectRegister
    \param AKey :: Map keyname
   */
{
  ELog::RegMethod RegA("FixedGroup","registerKey");

  CompTYPE FCUnit(new FixedUnit(FixedComp::unregistered,keyName+AKey));
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

std::set<std::string>
FixedGroup::getAllKeys() const
  /*!
    Accessor to all keys
   */
{
  std::set<std::string> units;
  for(const auto& [Name,FC] : FMap)
    units.emplace(Name);
  return units;
}
  
FixedComp&
FixedGroup::addKey(const std::string& Key)
  /*!
    Add a component to the group
    \param Key :: Key to add
    \return Fixed
  */
{
  ELog::RegMethod RegA("FixedGroup","addKey");

  FTYPE::iterator mc=FMap.find(Key);
  if (mc!=FMap.end())
    throw ColErr::InContainerError<std::string>
      (Key,"Key present in FMap:"+keyName);

  registerKey(Key);
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
  ELog::RegMethod RegA("FixedGroup","getPrimary");
  
  FTYPE::iterator mc=FMap.find(primKey); 
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(primKey,"Key in FMap:"+keyName);
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
  ELog::RegMethod RegA("FixedGroup","getPrimary(const)");
  
  FTYPE::const_iterator mc=FMap.find(primKey);  // can't fail
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(primKey,"Key in FMap:"+keyName);
  return *(mc->second);
}

FixedComp&
FixedGroup::getSecondary() 
  /*!
    Determine the component from the key
    \param Key :: Key to look up
    \return FixedComp 
  */
{
  ELog::RegMethod RegA("FixedGroup","getSecondary");
  
  FTYPE::iterator mc=FMap.find(sndKey);  // can't fail
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(sndKey,"Key in FMap:"+keyName);
  return *(mc->second);
}

const FixedComp&
FixedGroup::getSecondary() const
  /*!
    Determine the component from the key
    \param Key :: Key to look up
    \return FixedComp 
  */
{
  ELog::RegMethod RegA("FixedGroup","getSecondary(const)");
  
  FTYPE::const_iterator mc=FMap.find(sndKey);  // throws
  if (mc==FMap.end())
    throw ColErr::InContainerError<std::string>(sndKey,"Key in FMap:"+keyName);
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
    NOTE: secondaries and others not applied:
    \param FC :: Fixed Component (FixedGroup)
    \param sideIndex :: signed linkpoint			
  */
{
  ELog::RegMethod RegA("FixedGroup","createUnitVector");
  
  FixedGroup::createUnitVector(primKey,FC,sideIndex);  
  return;
}

void
FixedGroup::secondaryUnitVector(const attachSystem::FixedComp& FC,
				const long int sideIndex)
  /*!
    Simple way to call createUnitVector(second...)
    \param FC :: Fixed Component (FixedGroup)
    \param sideIndex :: signed linkpoint			
  */
{
  ELog::RegMethod RegA("FixedGroup","secondaryUnitVector");

  FixedGroup::createUnitVector(sndKey,FC,sideIndex);
  return;
}

void
FixedGroup::secondaryUnitVector(const attachSystem::FixedComp& FC,
				const std::string& sideName)
  /*!
    Simple way to call createUnitVector(second...)
    \param FC :: Fixed Component (FixedGroup)
    \param sideIndex :: signed linkpoint			
  */
{
  ELog::RegMethod RegA("FixedGroup","secondaryUnitVector(name)");

  // care here : need to call this for virtual table -- don't
  // directy call FixedGroup::createUnitVector.
  secondaryUnitVector(FC,FC.getSideIndex(sideName));

  return;
}

void
FixedGroup::createUnitVector(const std::string& unitName,
			     const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    Applies FC[grp](sideIndex) to selected component
    \param unitName :: Name of component
    \param FC :: Fixed Component (FixedGroup)
    \param sideIndex :: signed linkpoint			
  */
{
  ELog::RegMethod RegA("FixedGroup","createUnitVector(int)");

  attachSystem::FixedComp& activeFC=getKey(unitName);
  activeFC.createUnitVector(FC,sideIndex);
  setBeamCoord(unitName);

  return;
}

void
FixedGroup::setBeamCoord(const std::string& unitName)
  /*!
    Given a name set the bx value is the name is the
    secondary or primary.
    \param unitName :: name of the component begin set
  */
{
  ELog::RegMethod RegA("FixedGroup","setBeamCoord");

  if (unitName==primKey)
    {
      attachSystem::FixedComp& activeFC=getKey(unitName);
      X=activeFC.getX();
      Y=activeFC.getY();
      Z=activeFC.getZ();
      Origin=activeFC.getCentre();
    }
  if (unitName==sndKey)
    {
      attachSystem::FixedComp& activeFC=getKey(unitName);
      bX=activeFC.getX();
      bY=activeFC.getY();
      bZ=activeFC.getZ();
      bOrigin=activeFC.getCentre();


    }
  return;
}
  
  

}  // NAMESPACE attachSystem
