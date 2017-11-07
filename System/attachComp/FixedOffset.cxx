/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedOffset.cxx
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
#include "FixedOffset.h"

namespace attachSystem
{

FixedOffset::FixedOffset(const std::string& KN,const size_t NL) :
  FixedComp(KN,NL),
  preXYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xyAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
  */
{}

FixedOffset::FixedOffset(const FixedOffset& A) : 
  FixedComp(A),
  preXYAngle(A.preXYAngle),preZAngle(A.preZAngle),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle)
  /*!
    Copy constructor
    \param A :: FixedOffset to copy
  */
{}

FixedOffset&
FixedOffset::operator=(const FixedOffset& A)
  /*!
    Assignment operator
    \param A :: FixedOffset to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FixedComp::operator=(A);
      preXYAngle=A.preXYAngle;
      preZAngle=A.preZAngle;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
    }
  return *this;
}


void
FixedOffset::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedOffset","populate");

  // defaults used to fixedoffset can be used in a setting class.
  preXYAngle=Control.EvalDefVar<double>(keyName+"PreXYAngle",preXYAngle);
  preZAngle=Control.EvalDefVar<double>(keyName+"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefVar<Geometry::Vec3D>
    (keyName+"CentOffset",Geometry::Vec3D(xStep,yStep,zStep));
  
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();

  xStep=Control.EvalDefVar<double>(keyName+"XStep",xStep);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",yStep);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",zStep);
  
  xyAngle=Control.EvalDefVar<double>(keyName+"XYAngle",xyAngle);
  zAngle=Control.EvalDefVar<double>(keyName+"ZAngle",zAngle);
  
  return;
  
}

void
FixedOffset::populate(const std::string& baseName,
		      const FuncDataBase& Control)
  /*!
    Populate the variables
    \param baseName :: second default basename 
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedOffset","populate(baseName)");

  // defaults used to fixedoffset can be used in a setting class.
  preXYAngle=Control.EvalDefPair<double>(keyName,baseName,
					 "PreXYAngle",preXYAngle);
  preZAngle=Control.EvalDefPair<double>(keyName,baseName,"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefPair<Geometry::Vec3D>
    (keyName,baseName,"CentOffset",Geometry::Vec3D(0,0,0));
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();


  xStep=Control.EvalDefPair<double>(keyName,baseName,"XStep",xStep);
  yStep=Control.EvalDefPair<double>(keyName,baseName,"YStep",yStep);
  zStep=Control.EvalDefPair<double>(keyName,baseName,"ZStep",zStep);
  xyAngle=Control.EvalDefPair<double>(keyName,baseName,"XYAngle",xyAngle);
  zAngle=Control.EvalDefPair<double>(keyName,baseName,"ZAngle",zAngle);
  return;
  
}

void
FixedOffset::setPreRotation(const double XYA,const double ZA)
  /*!
    Set the Pre-rotation values 
    \param XYA :: xy angle rotation
    \param ZA :: xy angle rotation
   */
{
  preXYAngle=XYA;
  preZAngle=ZA;
  return;
}

void
FixedOffset::setRotation(const double XYA,const double ZA)
  /*!
    Set the Potation values 
    \param XYA :: xy angle rotation
    \param ZA :: xy angle rotation
   */
{
  xyAngle=XYA;
  zAngle=ZA;
  return;
}

void
FixedOffset::setOffset(const double XS,const double YS,const double ZS)
  /*!
    Set the Potation values 
    \param XS :: xStep
    \param YS :: yStep
    \param ZS :: zStep
   */
{
  xStep=XS;
  yStep=YS;
  zStep=ZS;
  return;
}
  
void
FixedOffset::applyOffset()
  /*!
    Apply the rotation/step offset
  */
{
  ELog::RegMethod RegA("FixedOffset","applyOffset");

  FixedComp::applyAngleRotate(preXYAngle,preZAngle);
  FixedComp::applyShift(xStep,yStep,zStep);
  FixedComp::applyAngleRotate(xyAngle,zAngle);
  FixedComp::reOrientate();      // this might still be active
  return;
}

void
FixedOffset::linkShift(const long int sideIndex)
  /*!
    Apply a rotation to a link point axis
    \param sideIndex :: link point index [signed]
  */
{
  ELog::RegMethod RegA("FixedOffset","applyOffset");

  FixedComp::linkShift(sideIndex,xStep,yStep,zStep);
  return;
}

void
FixedOffset::linkAngleRotate(const long int sideIndex)
  /*!
    Apply a rotation to a link point axis
    \param sideIndex :: link point index [signed]
   */
{
  ELog::RegMethod RegA("FixedOffset","applyOffset");

  FixedComp::linkAngleRotate(sideIndex,xyAngle,zAngle);
  return;
}



}  // NAMESPACE attachSystem
