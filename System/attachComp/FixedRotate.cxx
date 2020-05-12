/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/FixedRotate.cxx
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
#include "inputSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "FixedRotate.h"

namespace attachSystem
{

FixedRotate::FixedRotate(const std::string& KN,const size_t NL) :
  FixedComp(KN,NL),
  preXAngle(0.0),preYAngle(0.0),preZAngle(0.0),
  xStep(0.0),yStep(0.0),zStep(0.0),
  xAngle(0.0),yAngle(0.0),zAngle(0.0)
 /*!
    Constructor 
    \param KN :: KeyName
    \param NL :: Number of links
  */
{}

FixedRotate::FixedRotate(const FixedRotate& A) : 
  FixedComp(A),
  preXAngle(A.preXAngle),preYAngle(A.preYAngle),
  preZAngle(A.preZAngle),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xAngle(A.xAngle),yAngle(A.yAngle),zAngle(A.zAngle)
  /*!
    Copy constructor
    \param A :: FixedRotate to copy
  */
{}

FixedRotate&
FixedRotate::operator=(const FixedRotate& A)
  /*!
    Assignment operator
    \param A :: FixedRotate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      FixedComp::operator=(A);
      preXAngle=A.preXAngle;
      preYAngle=A.preYAngle;
      preZAngle=A.preZAngle;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xAngle=A.xAngle;
      yAngle=A.yAngle;
      zAngle=A.zAngle;
    }
  return *this;
}


void
FixedRotate::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedRotate","populate");

  // defaults used to fixedoffset can be used in a setting class.
  // note extra so can use a fixedoffset angle system if over-step
  preXAngle=Control.EvalDefVar<double>(keyName+"PreXYAngle",preXAngle);
  preXAngle=Control.EvalDefVar<double>(keyName+"PreXAngle",preXAngle);
  preYAngle=Control.EvalDefVar<double>(keyName+"PreYAngle",preYAngle);
  preZAngle=Control.EvalDefVar<double>(keyName+"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefVar<Geometry::Vec3D>
    (keyName+"CentOffset",Geometry::Vec3D(xStep,yStep,zStep));
  
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();

  xStep=Control.EvalDefVar<double>(keyName+"XStep",xStep);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",yStep);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",zStep);

  // note extra so can use a fixedoffset angle system if over-step
  xAngle=Control.EvalDefVar<double>(keyName+"XYAngle",xAngle);
  xAngle=Control.EvalDefVar<double>(keyName+"XAngle",xAngle);
  yAngle=Control.EvalDefVar<double>(keyName+"YAngle",yAngle);
  zAngle=Control.EvalDefVar<double>(keyName+"ZAngle",zAngle);
  
  return;
  
}

void
FixedRotate::populate(const std::map<std::string,
		      std::vector<std::string>>& inputMap)
  /*!
    Populate the variables
    \param inputMap :: Moifications from input stream
   */
{
  ELog::RegMethod RegA("FixedRotate","populate(map)");

  mainSystem::findInput(inputMap,"preXYAngle",0,preXAngle);
  mainSystem::findInput(inputMap,"preXAngle",0,preXAngle);
  mainSystem::findInput(inputMap,"preYAngle",0,preYAngle);
  mainSystem::findInput(inputMap,"preZAngle",0,preZAngle);
  mainSystem::findInput(inputMap,"xyAngle",0,xAngle);
  mainSystem::findInput(inputMap,"xAngle",0,xAngle);
  mainSystem::findInput(inputMap,"yAngle",0,yAngle);
  mainSystem::findInput(inputMap,"zAngle",0,zAngle);
  mainSystem::findInput(inputMap,"xStep",0,xStep);
  mainSystem::findInput(inputMap,"yStep",0,yStep);
  mainSystem::findInput(inputMap,"zStep",0,zStep);

  return;
}

void
FixedRotate::populate(const std::string& baseName,
		      const FuncDataBase& Control)
  /*!
    Populate the variables
    \param baseName :: second default basename 
    \param Control :: Control data base
   */
{
  ELog::RegMethod RegA("FixedRotate","populate(baseName)");

  // defaults used to fixedoffset can be used in a setting class.
  preXAngle=Control.EvalDefTail<double>(keyName,baseName,"PreXAngle",preXAngle);
  preYAngle=Control.EvalDefTail<double>(keyName,baseName,"PreYAngle",preYAngle);
  preZAngle=Control.EvalDefTail<double>(keyName,baseName,"PreZAngle",preZAngle);

  const Geometry::Vec3D CentOffset=Control.EvalDefTail<Geometry::Vec3D>
    (keyName,baseName,"CentOffset",Geometry::Vec3D(0,0,0));
  xStep=CentOffset.X();
  yStep=CentOffset.Y();
  zStep=CentOffset.Z();

  xStep=Control.EvalDefTail<double>(keyName,baseName,"XStep",xStep);
  yStep=Control.EvalDefTail<double>(keyName,baseName,"YStep",yStep);
  zStep=Control.EvalDefTail<double>(keyName,baseName,"ZStep",zStep);
  
  xAngle=Control.EvalDefTail<double>(keyName,baseName,"XAngle",xAngle);
  yAngle=Control.EvalDefTail<double>(keyName,baseName,"YAngle",yAngle);
  zAngle=Control.EvalDefTail<double>(keyName,baseName,"ZAngle",zAngle);
  return;
  
}

void
FixedRotate::setPreRotation(const double XA,const double YA,const double ZA)
  /*!
    Set the Pre-rotation values 
    \param XA :: x angle rotation
    \param YA :: y angle rotation
    \param ZA :: xy angle rotation
   */
{
  preXAngle=XA;
  preYAngle=YA;
  preZAngle=ZA;
  return;
}

void
FixedRotate::setRotation(const double XA,const double YA,
			 const double ZA)
  /*!
    Set the Potation values 
    \param XA :: x angle rotation
    \param YA :: y angle rotation
    \param ZA :: Z angle rotation
   */
{
  xAngle=XA;
  yAngle=YA;
  zAngle=ZA;
  return;
}

void
FixedRotate::setOffset(const double XS,const double YS,const double ZS)
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
FixedRotate::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: signed linkpt			
  */
{
  ELog::RegMethod RegA("FixedRotate","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
    
  return;
}

void
FixedRotate::createCentredUnitVector(const attachSystem::FixedComp& FC,
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

  Origin+=Y*(length/2.0);
  return;
}
  

void
FixedRotate::applyOffset()
  /*!
    Apply the rotation/step offset
  */
{
  ELog::RegMethod RegA("FixedRotate","applyOffset");
  
  FixedComp::applyAngleRotate(preXAngle,preYAngle,preZAngle);
  FixedComp::applyShift(xStep,yStep,zStep);
  FixedComp::applyAngleRotate(xAngle,yAngle,zAngle);
  FixedComp::reOrientate();      // this might still be active

  return;
}

void
FixedRotate::linkShift(const size_t sideIndex)
  /*!
    Apply a rotation to a link point axis
    \param sideIndex :: link point index [signed]
  */
{
  ELog::RegMethod RegA("FixedRotate","linkShift");

  FixedComp::linkShift(sideIndex,xStep,yStep,zStep);
  return;
}

void
FixedRotate::linkAngleRotate(const size_t sideIndex)
  /*!
    Apply a rotation to a link point axis
    \param sideIndex :: link point index [signed]
   */
{
  ELog::RegMethod RegA("FixedRotate","linkAngleRotate");

  FixedComp::linkAngleRotate(sideIndex,xAngle,yAngle,zAngle);
  return;
}



}  // NAMESPACE attachSystem
