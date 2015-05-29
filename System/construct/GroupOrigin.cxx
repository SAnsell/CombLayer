/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/GroupOrigin.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "GroupOrigin.h"

namespace constructSystem
{

GroupOrigin::GroupOrigin(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  grpIndex(ModelSupport::objectRegister::Instance().cell(Key))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

GroupOrigin::GroupOrigin(const GroupOrigin& A) : 
  attachSystem::FixedComp(A),
  grpIndex(A.grpIndex),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle)
  /*!
    Copy constructor
    \param A :: GroupOrigin to copy
  */
{}

GroupOrigin&
GroupOrigin::operator=(const GroupOrigin& A)
  /*!
    Assignment operator
    \param A :: GroupOrigin to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
    }
  return *this;
}


GroupOrigin::~GroupOrigin() 
 /*!
   Destructor
 */
{}

void
GroupOrigin::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control ::  DataBase of variables
 */
{
  ELog::RegMethod RegA("GroupOrigin","populate");
  
  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");  
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  return;
}
  
void
GroupOrigin::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("GroupOrigin","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  return;
}

void
GroupOrigin::createAll(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC)
		      
  /*!
    Global creation of the hutch
    \param Control :: Variable DataBase
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("GroupOrigin","createAll");
  populate(Control);
  createUnitVector(FC);

  return;
}

  
}  // NAMESPACE ts1System
