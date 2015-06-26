/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedOffsetGroup.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"

namespace attachSystem
{

FixedOffsetGroup::FixedOffsetGroup(const std::string& mainKey,
				   const std::string& KN,
				   const size_t NL) :
  FixedGroup(mainKey,KN,NL),xStep(0.0),yStep(0.0),zStep(0.0),
  xyAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
  */
{}

FixedOffsetGroup::FixedOffsetGroup(const std::string& mainKey,
				   const std::string& AKey,
				   const size_t ANL,
				   const std::string& BKey,
				   const size_t BNL) :
  FixedGroup(mainKey,AKey,ANL,BKey,BNL),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xyAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param mainKey :: KeyName [main system]
    \param AKey :: Key unit
    \param ANL :: A Number of links
    \param BKey :: Key unit
    \param BNL :: B Number of links
  */
{}

 FixedOffsetGroup::FixedOffsetGroup(const std::string& mainKey,
				    const std::string& AKey,
				    const size_t ANL,
				    const std::string& BKey,
				    const size_t BNL,
				    const std::string& CKey,
				    const size_t CNL) :
  FixedGroup(mainKey,AKey,ANL,BKey,BNL,CKey,CNL),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xyAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param mainKey :: KeyName [main system]
    \param AKey :: Key unit
    \param ANL :: A Number of links
    \param BKey :: Key unit
    \param BNL :: B Number of links
    \param CKey :: Key unit
    \param CNL :: B Number of links
  */
{}

void
FixedOffsetGroup::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedOffsetGroup","populate");

  xStep=Control.EvalDefVar<double>(keyName+"XStep",0.0);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",0.0);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",0.0);
  xyAngle=Control.EvalDefVar<double>(keyName+"XYAngle",0.0);
  zAngle=Control.EvalDefVar<double>(keyName+"ZAngle",0.0);
  return;
}
  
void
FixedOffsetGroup::applyOffset()
  /*!
    Apply the rotation/step offset
  */
{
  ELog::RegMethod RegA("FixedOffsetGroup","applyOffset");

  for(FTYPE::value_type& FCmc : FMap)
    {
      FCmc.second->applyShift(xStep,yStep,zStep);
      FCmc.second->applyAngleRotate(xyAngle,zAngle);
    }
  return;
}

 
}  // NAMESPACE attachSystem
