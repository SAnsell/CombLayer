/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedOffset.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "inputSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "FixedOffset.h"

namespace attachSystem
{

FixedOffset::FixedOffset(const size_t NL,const std::string& KN) :
  FixedComp(NL,KN),
  preXYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xyAngle(0.0),zAngle(0.0)
 /*!
    Null version Constructor 
    \param NL :: Number of links
    \param KN :: KeyName [not registered]
  */
{}

FixedOffset::FixedOffset(const size_t NL) :
  FixedOffset(NL,"Null")
 /*!
    Null version Constructor 
    \param NL :: Number of links
  */
{}

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

FixedOffset::FixedOffset(const std::string& KN,const size_t NL,
			 const size_t resSize) :
  FixedComp(KN,NL,resSize),
  preXYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xyAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
    \param resSize :: Reserved space
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
    (keyName+"Offset",Geometry::Vec3D(xStep,yStep,zStep));
  
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
FixedOffset::populate(const std::map<std::string,
		      std::vector<std::string>>& inputMap)
  /*!
    Populate the variables
    \param inputMap :: Moifications from input stream
   */
{
  ELog::RegMethod RegA("FixedOffset","populate(map)");

  Geometry::Vec3D CentOffset(xStep,yStep,zStep);
				
  mainSystem::findInput(inputMap,"Offset",0,CentOffset);
  
  mainSystem::findInput(inputMap,"preXYAngle",0,preXYAngle);
  mainSystem::findInput(inputMap,"preZAngle",0,preZAngle);
  mainSystem::findInput(inputMap,"xyAngle",0,xyAngle);
  mainSystem::findInput(inputMap,"zAngle",0,zAngle);

  xStep=CentOffset[0];
  yStep=CentOffset[1];
  zStep=CentOffset[2];
  mainSystem::findInput(inputMap,"xStep",0,xStep);
  mainSystem::findInput(inputMap,"yStep",0,yStep);
  mainSystem::findInput(inputMap,"zStep",0,zStep);
  
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
  preXYAngle=Control.EvalDefTail<double>(keyName,baseName,
					 "PreXYAngle",preXYAngle);
  preZAngle=Control.EvalDefTail<double>(keyName,baseName,"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefTail<Geometry::Vec3D>
    (keyName,baseName,"Offset",Geometry::Vec3D(0,0,0));
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();

  xStep=Control.EvalDefTail<double>(keyName,baseName,"XStep",xStep);
  yStep=Control.EvalDefTail<double>(keyName,baseName,"YStep",yStep);
  zStep=Control.EvalDefTail<double>(keyName,baseName,"ZStep",zStep);
  xyAngle=Control.EvalDefTail<double>(keyName,baseName,"XYAngle",xyAngle);
  zAngle=Control.EvalDefTail<double>(keyName,baseName,"ZAngle",zAngle);
  return;
  
}

void
FixedOffset::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: signed linkpt			
  */
{
  ELog::RegMethod RegA("FixedOffset","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
    
  return;
}

void
FixedOffset::createCentredUnitVector(const attachSystem::FixedComp& FC,
				     const long int sideIndex,
				     const double length)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: signed linkpt			
    \param length :: full length of object
  */
{
  ELog::RegMethod RegA("FixedRotate","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  Origin+=Y*length;
  return;
}

void
FixedOffset::setPreRotation(const double XYA,const double ZA)
  /*!
    Set the Pre-rotation values 
    \param XYA :: xy angle rotation
    \param ZA :: z angle rotation
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
FixedOffset::linkShift(const size_t sideIndex)
  /*!
    Apply a rotation to a link point axis
    \param sideIndex :: link point index [signed]
  */
{
  ELog::RegMethod RegA("FixedOffset","linkShift");

  FixedComp::linkShift(sideIndex,xStep,yStep,zStep);
  return;
}

void
FixedOffset::linkAngleRotate(const size_t sideIndex)
  /*!
    Apply a rotation to a link point axis
    \param sideIndex :: link point index [signed]
   */
{
  ELog::RegMethod RegA("FixedOffset","lnkAngleRotate");

  FixedComp::linkAngleRotate(sideIndex,xyAngle,zAngle);
  return;
}



}  // NAMESPACE attachSystem
