/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/FocusPoints.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <array>
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
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FocusPoints.h"

namespace essSystem
{

FocusPoints::FocusPoints(const std::string& Key) :
  attachSystem::FixedOffset(Key,4)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister::Instance().cell(keyName);
}

FocusPoints::FocusPoints(const FocusPoints& A) : 
  attachSystem::FixedOffset(A),
  forwardDist(A.forwardDist),backDist(A.backDist),
  leftWidth(A.leftWidth),rightWidth(A.rightWidth)
  /*!
    Copy constructor
    \param A :: FocusPoints to copy
  */
{}

FocusPoints&
FocusPoints::operator=(const FocusPoints& A)
  /*!
    Assignment operator
    \param A :: FocusPoints to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      forwardDist=A.forwardDist;
      backDist=A.backDist;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
    }
  return *this;
}

FocusPoints*
FocusPoints::clone() const
  /*!
    virtual copy constructor
    \return new FocusPoints(*this)
  */
{
  return new FocusPoints(*this);
}

 
FocusPoints::~FocusPoints()
  /*!
    Destructor
  */
{}

void
FocusPoints::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("FocusPoints","populate");

  FixedOffset::populate(Control);
  
  forwardDist=Control.EvalPair<double>(keyName+"ForwardDistance",
                                       keyName+"Distance");
  backDist=Control.EvalPair<double>(keyName+"BackDistance",keyName+"Distance");
  leftWidth=Control.EvalPair<double>(keyName+"leftWidth",keyName+"Width");
  rightWidth=Control.EvalPair<double>(keyName+"rightWidth",keyName+"Width");
  return;
}

void
FocusPoints::createUnitVector(const attachSystem::FixedComp& axisFC,
				     const long int sideIndex)
  /*!
    Create the unit vectors. This one uses axis from ther first FC
    but the origin for the second. Futher shifting the origin on the
    Z axis to the centre.
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param sideIndex :: link point for origin if given
  */
{
  ELog::RegMethod RegA("FocusPoints","createUnitVector");

  attachSystem::FixedComp::createUnitVector(axisFC,sideIndex);
  applyOffset();
  return;
}


void
FocusPoints::createLinks()
  /*!
    Create links but currently incomplete
  */
{
  ELog::RegMethod RegA("FocusPoints","createLinks");

  FixedComp::setConnect(0,Origin-Y*backDist-X*leftWidth,-Y);
  FixedComp::setConnect(1,Origin-Y*backDist+X*rightWidth,-Y);
  FixedComp::setConnect(2,Origin+Y*forwardDist-X*leftWidth,Y);
  FixedComp::setConnect(3,Origin+Y*forwardDist+X*rightWidth,Y);

  return;
}

  

  
void
FocusPoints::createAll(Simulation& System,
                       const attachSystem::FixedComp& axisFC,
                       const long int sideIndex)
  /*!
    Construct the butterfly components
    \param System :: Simulation 
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("FocusPoints","createAll");

  populate(System.getDataBase());
  createUnitVector(axisFC,sideIndex);
  createLinks();
  
  return;
}


}  // NAMESPACE essSystem
