/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/RElement.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <boost/multi_array.hpp>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"

#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "RElement.h"

namespace delftSystem
{

RElement::RElement(const size_t XI,const size_t YI,
		   const std::string& Key) : 
  attachSystem::FixedOffset(ReactorGrid::getElementName(Key,XI,YI),6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  baseName(Key),XIndex(XI),YIndex(YI)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param XI :: Grid element index
    \param YI :: Grid element index
    \param Key :: KeyName
  */
{}

RElement::RElement(const RElement& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),baseName(A.baseName),
  XIndex(A.XIndex),YIndex(A.YIndex),
  insertCell(A.insertCell)
  /*!
    Copy constructor
    \param A :: RElement to copy
  */
{}

RElement&
RElement::operator=(const RElement& A)
  /*!
    Assignment operator
    \param A :: RElement to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      insertCell=A.insertCell;
    }
  return *this;
}

void
RElement::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    Requires that unset values are copied from previous block
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("RElement","populate");

  xStep=ReactorGrid::getElement<double>(Control,baseName+"XStep",
					XIndex,YIndex);
  yStep=ReactorGrid::getElement<double>(Control,baseName+"YStep",
					XIndex,YIndex);
  zStep=ReactorGrid::getElement<double>(Control,baseName+"ZStep",
					XIndex,YIndex);

  xyAngle=ReactorGrid::getElement<double>(Control,baseName+"XYAngle",
					  XIndex,YIndex);
  zAngle=ReactorGrid::getElement<double>(Control,baseName+"ZAngle",
					 XIndex,YIndex);

  return;
}

std::string
RElement::getItemKeyName() const
  /*!
    Return the specific item name
    \return full keyname
   */
{
  return ReactorGrid::getElementName(keyName,XIndex,YIndex);
}



} // NAMESPACE delftSystem
