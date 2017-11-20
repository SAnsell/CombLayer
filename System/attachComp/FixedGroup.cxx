/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedGroup.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "Rules.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "objectRegister.h"
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
}

FixedGroup::FixedGroup(const FixedGroup& A) : 
  FixedComp(A),FMap(A.FMap)
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
      FMap=A.FMap;
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

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.cell(keyName+AKey);
  CompTYPE FCUnit(new FixedComp(keyName+AKey,NL));  
  OR.addObject(FCUnit);
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


  

}  // NAMESPACE attachSystem
