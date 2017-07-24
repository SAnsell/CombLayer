/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   funcBase/FFunc.cxx
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

//-----------------------------------------
// FFunc
//-----------------------------------------

FFunc::FFunc(varList* VA,const int I,const Code& CObj) :
  FItem(VA,I),BaseUnit(CObj)
  /*!
    Standard constructor
    \param VA :: VarList pointer
    \param I :: Index Item
    \param CObj :: Code Item to allow evaluation
  */
{}

FFunc::FFunc(const FFunc& A) :
  FItem(A),BaseUnit(A.BaseUnit)
  /*!
    Standard copy constructor
    \param A :: FFunc object to copy
  */
{}

FFunc&
FFunc::operator=(const FFunc& A)
  /*!
    Standard Assignment operator 
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FItem::operator=(A);
      BaseUnit=A.BaseUnit;
    }
  return *this;
}


FFunc*
FFunc::clone() const
  /*!
    Virtual constructor
    \return new FFunc(this)
  */
{
  return new FFunc(*this);
}

FFunc::~FFunc()
  /*!
    Standard Destructor 
  */
{}

void
FFunc::setValue(const Code& AC)
  /*!
    Sets the base item (code)
    \param AC :: Code Item
  */
{
  BaseUnit=AC;
  return;
}

int
FFunc::getValue(Geometry::Vec3D& V) const
  /*!
    Get the values. Note that this
    uses varlist. Note that must deal with situation that 
    we can't easily convert to Vec3D. Since it is used in the
    Code section

    \param V :: Output value
    \return 1 if appropiate eval / 0 otherwise
  */
{
  ELog::RegMethod RegA("FFunc","getValue(Vec3D)");
  Code BC(BaseUnit);
  try
    {
      V=BC.Eval<Geometry::Vec3D>(FItem::VListPtr);
    }
  catch(ColErr::TypeConvError<double,Geometry::Vec3D>&)
    {
      return 0;
    }
  const_cast<int&>(active)++;
  return 1;
}

int
FFunc::getValue(double& V) const
  /*!
    Get the values. Note that this
    uses varlist
    \return 1 if appropiate eval / 0 otherwise
  */
{
  Code BC(BaseUnit);
  V=BC.Eval<double>(FItem::VListPtr);
  const_cast<int&>(active)++;
  return 1;
}

int
FFunc::getValue(int& V) const
  /*!
    Get the values. Note that this
    uses varlist
    \return Code expression 
  */
{
  Code BC(BaseUnit);
  V=static_cast<int>(BC.Eval<double>(VListPtr));
  const_cast<int&>(active)++;
  return 1;
}

int
FFunc::getValue(long int& V) const
  /*!
    Get the values. Note that this
    uses varlist
    \return Code expression 
  */
{
  Code BC(BaseUnit);
  V=static_cast<long int>(BC.Eval<double>(VListPtr));
  const_cast<int&>(active)++;
  return 1;
}


int
FFunc::getValue(size_t& V) const
  /*!
    Get the values. Note that this
    uses varlist
    \return Code expression 
  */
{
  Code BC(BaseUnit);
  V=static_cast<size_t>(BC.Eval<double>(VListPtr));
  const_cast<int&>(active)++;
  return 1;
}

int 
FFunc::getValue(std::string& V) const
  /*!
    Get the values. Note that this
    uses varlist
    \return Code expression 
  */
{
  Code BC(BaseUnit);
  double Val=BC.Eval<double>(VListPtr);
  std::stringstream cx;
  cx<<Val;
  V=cx.str();
  const_cast<int&>(active)++;
  return 1;
}

std::string 
FFunc::typeKey() const
  /// type name
{  return "Code"; }

void
FFunc::write(std::ostream& OX) const
  /*!
    Write out the variable
    \param OX :: Output stream
   */
{
  OX<<"code :";
  BaseUnit.writeCompact(OX);
  return;
}

