/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   funcBase/FItem.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"

FItem::FItem(varList* VA,const int I) : 
  index(I),active(0),VListPtr(VA)
  /*!
    Standard Constructor
    \param VA :: VarList pointer
    \param I :: Index value
  */
{}

FItem::FItem(const FItem& A) :
  index(A.index),active(A.active),
  VListPtr(A.VListPtr)
  /*!
    Standard Copy Constructor
    \param A :: FItem to copy
  */
{ }

FItem& 
FItem::operator=(const FItem& A)
  /*!
    Standard assignment constructor
    \param A :: FItem to copy
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      active=A.active;
      VListPtr=A.VListPtr;
    }
  return *this;
}

FItem::~FItem()
 /*!
   Destructor
 */
{}

void 
FItem::setValue(const size_t&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(size_t)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

void 
FItem::setValue(const long int&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(long int)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

void 
FItem::setValue(const Geometry::Vec3D&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(Vec3D)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

void 
FItem::setValue(const double&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(double)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

void 
FItem::setValue(const int&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(int)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

void 
FItem::setValue(const std::string&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(string)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

void 
FItem::setValue(const Code&)
  /*!
    Default setValue throws 
    ExBase error 
  */
{
  ELog::RegMethod RegA("FItem","setValue(Code)");
  throw ColErr::ExBase(0,"Incorrect Type");
  return;
}

