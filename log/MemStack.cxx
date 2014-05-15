/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/MemStack.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "MemStack.h"

#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"

namespace ELog
{

MemStack::MemStack() : 
  verboseFlag(0),objCnt(0)
  /*!
    Constructor
  */
{}

MemStack::~MemStack()
  /*!
    Destructor
  */
{
  ELog::RegMethod RegA("MemStack","Destructor");

  if (!Key.empty())
    {
      ELog::EM<<"Undeleted memory "<<ELog::endCrit;
      write(ELog::EM.Estream());
      ELog::EM<<" ---------------- "<<ELog::endCrit;
    }
  if (verboseFlag)
    ELog::EM<<"Total objects "<<objCnt<<ELog::endTrace;
}

MemStack&
MemStack::Instance()
  /*!
    Memstack Accessor [Singleton]
    \return effective this
   */
{
  static MemStack A;
  return A;
}

void
MemStack::clear()
 /*!
   Clear the stack
 */
{
  Key.erase(Key.begin(),Key.end());
  objCnt=0;
  return;
}

void
MemStack::addMem(const std::string& CN,
		 const std::string& MN,
		 const size_t& Address)
  /*!
    Adds a component to the class names series
    \param CN :: Class name
    \param MN :: Method name / Allocation place
    \param Address :: new memory location
  */
{
  MTYPE::const_iterator mc;
  mc=Key.find(Address);
  if (mc!=Key.end())
    {
      ELog::EM<<"Found reallocation \n";
      ELog::EM<<"Address == "<<Address<<" at "<<CN<<":"<<MN<<ELog::endCrit;
      ELog::EM<<"Previous == "<<mc->second.first<<" "
	      <<mc->second.second<<ELog::endErr;
      return;
    }
  Key.insert(MTYPE::value_type(Address,InnerTYPE(CN,MN)));
  objCnt++;
  if (verboseFlag>1)
    ELog::EM<<"Create "<<CN<<" "<<MN<<"::"
	    <<std::hex<<"0x"<<Address<<std::dec<<ELog::endTrace;
  

  return;
}

void
MemStack::delMem(const size_t& Address)
  /*!
    Removes a component to the class names series
    \param Address :: new memory location
  */
{
  MTYPE::iterator mc;
  mc=Key.find(Address);
  if (mc==Key.end())
    {
      ELog::EM<<"Deleting unknown memory \n";
      ELog::EM<<"Address == "<<Address<<ELog::endErr;
    }
  else
    {
      if (verboseFlag>1)
	ELog::EM<<"Delete "<<mc->second.first
		<<" "<<mc->second.second<<"::"
		<<std::hex<<"Ox"<<mc->first
		<<std::dec<<ELog::endTrace;
      Key.erase(mc);
    }
  return;
}

void
MemStack::write(std::ostream& OX) const
  /*!
    Write out the undeleted object
    \param OX :: Output stream
  */
{
  MTYPE::const_iterator mc;
  for(mc=Key.begin();mc!=Key.end();mc++)
    {
      OX<<"Obj Ox"<<std::hex<<mc->first<<" :: "<<mc->second.first
	<<" ++ "<<mc->second.second<<std::dec<<"\n";
      
      const Geometry::Plane* Ptr=
	reinterpret_cast<const Geometry::Plane*>(mc->first);
      ELog::EM<<"Name == "<<Ptr->getName()<<ELog::endWarn;
    }
  return;
}



} // NameSpace ELog
