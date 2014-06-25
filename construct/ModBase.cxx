/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/ModBase.cxx
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "ModBase.h"

namespace constructSystem
{

ModBase::ModBase(const std::string& Key,const size_t nLinks)  :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(Key,nLinks),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param NLink :: Number of links
  */
{}

ModBase::ModBase(const ModBase& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),attachSystem::FixedComp(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle)
  /*!
    Copy constructor
    \param A :: ModBase to copy
  */
{}

ModBase&
ModBase::operator=(const ModBase& A)
  /*!
    Assignment operator
    \param A :: ModBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
    }
  return *this;
}


ModBase::~ModBase() 
 /*!
   Destructor
 */
{}


void
ModBase::populate(const FuncDataBase& Control)
  /*!
    Local populations
    \param Control :: FuncDAtabase to use
   */
{
  ELog::RegMethod RegA("ModBase","populate");

  const size_t NFlight=Control.EvalDefVar<size_t>(keyName+"NFlight",0);  
  flightSides.clear();
  for(size_t i=0;i<NFlight;i++)
    {
      const std::string kName=keyName+"FlightSide"+
	StrFunc::makeString(i);
      const long int I=Control.EvalVar<long int>(kName);  
      flightSides.push_back(I);
    }

  return;
}

std::string
ModBase::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,modIndex,surfList);
}


long int 
ModBase::getSideIndex(const size_t I) const
  /*!
    Accessor to flight sides
    \param I :: Index for side
   */
{
  return (I>=flightSides.size()) ? 0 : flightSides[I];
}

  
}  // NAMESPACE constructSystem
