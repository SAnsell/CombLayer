/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/EssModBase.cxx
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
#include <string>
#include <algorithm>
#include <numeric>
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "FixedOffset.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "EssModBase.h"

namespace essSystem
{

EssModBase::EssModBase(const std::string& Key,const size_t nLinks)  :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0,0),
  attachSystem::FixedOffset(Key,nLinks),attachSystem::CellMap(),
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param nLinks :: Number of links
  */
{}

EssModBase::EssModBase(const EssModBase& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex)
  /*!
    Copy constructor
    \param A :: EssModBase to copy
  */
{}

EssModBase&
EssModBase::operator=(const EssModBase& A)
  /*!
    Assignment operator
    \param A :: EssModBase to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
    }
  return *this;
}


EssModBase::~EssModBase() 
 /*!
   Destructor
 */
{}


void
EssModBase::populate(const FuncDataBase& Control)
  /*!
    Local populations
    \param Control :: FuncDatabase to use
   */
{
  ELog::RegMethod RegA("EssModBase","populate");

  FixedOffset::populate(Control);
  return;
}

void
EssModBase::createUnitVector(const attachSystem::FixedComp& orgFC,
                             const long int orgIndex,
                             const attachSystem::FixedComp& axisFC,
                             const long int axisIndex)
  /*!
    Local create unit for central Base.
    \param orgFC :: centre point of coordinate system
    \param axisFC :: Axis coordinate system
    \param orgIndex :: link point of orgFC
    \param axisIndex :: link point of axisFC
  */
{
  ELog::RegMethod RegA("EssModBase","createUnitVector");
  
  attachSystem::FixedComp::createUnitVector(axisFC,axisIndex);
  Origin=orgFC.getLinkPt(orgIndex);
  applyOffset();
  return; 
}

const attachSystem::FixedComp&
EssModBase::getComponent(const std::string& compName) const
  /*!
    Simple way to get a named component of this object
    \param compName :: Component name
    \return FixedComp object
  */
{
  ELog::RegMethod RegA("EssModBase","getComponent");

  throw ColErr::InContainerError<std::string>(compName,keyName+" component");
}

    
}  // NAMESPACE essSystem
