/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaMagnetic/magnetUnit.cxx
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
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "stringCombine.h"
#include "writeSupport.h"
#include "Surface.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h" 
#include "FixedComp.h"
#include "FixedOffset.h"
#include "magnetUnit.h"

namespace flukaSystem
{

magnetUnit::magnetUnit(const std::string& Key,
		       const size_t I) :
  attachSystem::FixedOffset(Key+std::to_string(I),0),
  index(I),length(0.0),width(0.0),height(0.0)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param I :: Index number
  */
{}

magnetUnit::magnetUnit(const magnetUnit& A) : 
  attachSystem::FixedOffset(A),
  index(A.index),length(A.length),width(A.width),
  height(A.height),activeCells(A.activeCells)
  /*!
    Copy constructor
    \param A :: magnetUnit to copy
  */
{}

magnetUnit&
magnetUnit::operator=(const magnetUnit& A)
  /*!
    Assignment operator
    \param A :: magnetUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      index=A.index;
      length=A.length;
      width=A.width;
      height=A.height;
      activeCells=A.activeCells;
    }
  return *this;
}


magnetUnit::~magnetUnit()
  /*!
    Destructor
   */
{}


void
magnetUnit::populate(const FuncDataBase& Control)
  /*!
    If the object is created as a normal object populate
    variables
    \param Control :: DataBase for variaibles
   */
{
  ELog::RegMethod RegA("magnetUnit","populate");
  
  FixedOffset::populate(Control);

  length=Control.EvalDefVar<double>(keyName+"Length",length);
  height=Control.EvalDefVar<double>(keyName+"Height",height);
  width=Control.EvalDefVar<double>(keyName+"Width",width);
  
  return;
}

void
magnetUnit::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("magnetUnit","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  

  return;
}

void
magnetUnit::createUnitVector(const Geometry::Vec3D& OG,
			     const Geometry::Vec3D& AY,
			     const Geometry::Vec3D& AZ)
  /*!
    Create the unit vectors.
    \param OG :: New origin
    \param AY :: Y Axis
    \param AZ :: Z Axis [reothorgalized]
  */
{
  ELog::RegMethod RegA("magnetUnit","createUnitVector");

  attachSystem::FixedComp::createUnitVector(OG,AY,AZ);
  applyOffset();  

  return;
}

void
magnetUnit::setExtent(const double EX,const double EY,
		      const double EZ)
  /*!
    Set the extent based on the vectorized distances
    \param EX :: Width
    \param EY :: Length
    \param EZ :: Height
  */
{
  if (EX>Geometry::zeroTol &&
      EY>Geometry::zeroTol &&
      EZ>Geometry::zeroTol)
    {
      length=EY;
      width=EX;
      height=EZ;
    }
  else
    {
      length=0.0;
      width=0.0;
      height=0.0;
    }
  return;
}
  
void
magnetUnit::addCell(const int CN)
  /*!
    Add cell to system
    \param CN :: Cell number
   */
{
  activeCells.insert(CN);
  return;
}

void
magnetUnit::writeFLUKA(std::ostream& OX) const
  /*!
    Write out the magnetic unit
    Structure is :
    - 0 x y z :: [origin]
    - 2 x1 x2 x3 :: [x axis]
    - 3 y1 y2 y3 :: [y axis]
    - 4 z1 z2 x3 :: [z axis]
    - 5 extX extY extZ :: [Extent vector [optional]]
    - 6 kValue :: [kValue]
    
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("magnetUnit","writeFLUKA");

  std::ostringstream cx;
  cx<<"USRICALL 0 "<<StrFunc::makeString(Origin)<<" - - "<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL 1 "<<StrFunc::makeString(X)<<" - - "<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL 2 "<<StrFunc::makeString(Y)<<" - - "<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL 3 "<<StrFunc::makeString(Z)<<" - - "<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  if (length>Geometry::zeroTol &&
      width>Geometry::zeroTol &&
      height>Geometry::zeroTol)
    {
      cx.str("");
      cx<<"USRICALL 4 "<<StrFunc::makeString(width)<<" "
	<<StrFunc::makeString(length)<<" "
	<<StrFunc::makeString(height)<<" - - "
	<<keyName;
      StrFunc::writeFLUKA(cx.str(),OX);
    }
 
  return;
}
  

}  // NAMESPACE flukaSystem
