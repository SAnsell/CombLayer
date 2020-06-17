/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/subPipeUnit.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <array>

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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "MaterialSupport.h"

#include "subPipeUnit.h"

namespace tdcSystem
{

subPipeUnit::subPipeUnit(const subPipeUnit& A) : 
  xStep(A.xStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),radius(A.radius),length(A.length),
  thick(A.thick),flangeRadius(A.flangeRadius),flangeLength(A.flangeLength),
  voidMat(A.voidMat),wallMat(A.wallMat),flangeMat(A.flangeMat)
  /*!
    Copy constructor
    \param A :: subPipeUnit to copy
  */
{}

subPipeUnit&
subPipeUnit::operator=(const subPipeUnit& A)
  /*!
    Assignment operator
    \param A :: subPipeUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      xStep=A.xStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      radius=A.radius;
      length=A.length;
      thick=A.thick;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      voidMat=A.voidMat;      
      wallMat=A.wallMat;
      flangeMat=A.flangeMat;
    }
  return *this;
}
  
void
subPipeUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("subPipeUnit","populate");
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  thick=Control.EvalVar<double>(keyName+"Thick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}
    
  
}  // NAMESPACE tdcSystem
