/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/objectRegister.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "objectRegister.h"

namespace ModelSupport
{

objectRegister::objectRegister() : 
  GPtr(0)
  /*!
    Constructor
  */
{}

objectRegister::~objectRegister() 
  /*!
    Destructor
  */
{}

  
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

bool
objectRegister::hasGroup(const std::string& grpName) const
  /*!
    Simple accessor to determin if a group is register.
    The group is likely to be a FixedComp unit and all resultant
    objects
   */
{
  return (!GPtr) ? 0 : GPtr->hasObject(grpName);
}
  
void
objectRegister::setObjectGroup(objectGroups& OGrp)
  /*!
    Set the object group
    \param OGrp :: Object group [typcially simulation]
   */
{
  GPtr=&OGrp;
  return;
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

  if (GPtr)
    GPtr->addObject(Name,Ptr);
  return;
}

void
objectRegister::reAddObject(const std::string& Name,
			    const CTYPE& Ptr)
  /*!
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) if it exist in data base replace
    All failures result in an exception.
    \param Name :: Name of the object						
    \param Ptr :: Shared_ptr
  */
{
  ELog::RegMethod RegA("objectRegister","reAddObject");

  if (GPtr)
    GPtr->reAddObject(Name,Ptr);
  return;
}

void
objectRegister::reAddObject(const CTYPE& Ptr)
  /*! 
    Register a shared_ptr of an object. 
    Requirement that 
    - (a) The object already exists as a range
    - (b) No repeat object
    All failures result in an exception.
    \param Ptr :: FixedComp object [shared_ptr]
  */
{
  ELog::RegMethod RegA("objectRegister","reAddObject(Obj)");
  if (Ptr)
    reAddObject(Ptr->getKeyName(),Ptr);
  else
    throw ColErr::EmptyValue<void>("Ptr Shared_ptr");
  return;
}
  
  
  
int
objectRegister::cell(const std::string& Name,const size_t size)
  /*!
    Add a component and get a new cell number 
    \param Name :: Name of the unit
    \param size :: Size of unit to register
    \return the start number of the cellvalue
  */
{
  ELog::RegMethod RegA("objectRegister","cell");

  if (!GPtr)
    throw ColErr::EmptyValue<objectGroups*>("orderGroup Unregistered");
  return GPtr->cell(Name,size);
}

void
objectRegister::removeCell(const std::string& Name)
  /*!
    Remove a component and get clear cell number
    \param Name :: Name of the unit
    \return the start number of the cellvalue
  */
{
  ELog::RegMethod RegA("objectRegister","cell");

  if (!GPtr)
    throw ColErr::EmptyValue<objectGroups*>("orderGroup Unregistered");
  
  GPtr->removeObject(Name);
  return;
}


  
} // NAMESPACE ModelSupport
