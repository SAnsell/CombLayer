/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/RectPipeGenerator.cxx
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
#include <stack>
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
#include "surfEqual.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "RectPipeGenerator.h"

namespace setVariable
{

RectPipeGenerator::RectPipeGenerator() :
  pipeWidth(16.0),pipeHeight(16.0),pipeThick(0.5),
  flangeWidth(24.0),flangeHeight(24.0),flangeLen(1.0),
  windowWidth(12.0),windowHeight(12.0),windowThick(0.5),
  pipeMat("Aluminium"),windowMat("Silicon300K"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

RectPipeGenerator::RectPipeGenerator(const RectPipeGenerator& A) : 
  pipeWidth(A.pipeWidth),pipeHeight(A.pipeHeight),
  pipeThick(A.pipeThick),flangeWidth(A.flangeWidth),
  flangeHeight(A.flangeHeight),flangeLen(A.flangeLen),
  windowWidth(A.windowWidth),windowHeight(A.windowHeight),
  windowThick(A.windowThick),pipeMat(A.pipeMat),
  windowMat(A.windowMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: RectPipeGenerator to copy
  */
{}

RectPipeGenerator&
RectPipeGenerator::operator=(const RectPipeGenerator& A)
  /*!
    Assignment operator
    \param A :: RectPipeGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      pipeWidth=A.pipeWidth;
      pipeHeight=A.pipeHeight;
      pipeThick=A.pipeThick;
      flangeWidth=A.flangeWidth;
      flangeHeight=A.flangeHeight;
      flangeLen=A.flangeLen;
      windowWidth=A.windowWidth;
      windowHeight=A.windowHeight;
      windowThick=A.windowThick;
      pipeMat=A.pipeMat;
      windowMat=A.windowMat;
      voidMat=A.voidMat;
    }
  return *this;
}
  
RectPipeGenerator::~RectPipeGenerator() 
 /*!
   Destructor
 */
{}


void
RectPipeGenerator::setPipe(const double W,const double H,
                           const double T)
  /*!
    Set all the pipe values
    \param W :: full width of main pipe
    \param H :: full height of main pipe
    \param T :: Thickness
   */
{
  pipeWidth=W;
  pipeHeight=H;
  pipeThick=T;
  return;
}

void
RectPipeGenerator::setFlange(const double W,const double H,
                           const double L)
  /*!
    Set all the flange values
    \param W :: full width of main flange
    \param H :: full height of main flange
    \param L :: length
   */
{
  flangeWidth=W;
  flangeHeight=H;
  flangeLen=L;
  return;
}

void
RectPipeGenerator::setWindow(const double W,const double H,
                           const double T)
  /*!
    Set all the window values
    \param W :: full width of main window
    \param H :: full height of main window
    \param T :: Thickness
   */
{
  windowWidth=W;
  windowHeight=H;
  windowThick=T;
  return;
}

  
void
RectPipeGenerator::generatePipe(FuncDataBase& Control,
                                const std::string& keyName,
                                const double yStep,
                                const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("RectPipeGenerator","generatorPipe");

  const double realFlangeWidth=
    (flangeWidth<0.0) ? pipeWidth-flangeWidth : flangeWidth;
  const double realFlangeHeight=
    (flangeHeight<0.0) ? pipeHeight-flangeHeight : flangeHeight;
  const double realWindowWidth=
    (windowWidth<0.0) ? pipeWidth-windowWidth : windowWidth;
  const double realWindowHeight=
    (windowHeight<0.0) ? pipeHeight-windowHeight : windowHeight;

    // VACUUM PIPES:
  Control.addVariable(keyName+"YStep",yStep);   // step + flange
  
  Control.addVariable(keyName+"Width",pipeWidth);
  Control.addVariable(keyName+"Height",pipeHeight);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FeThick",pipeThick);
  
  Control.addVariable(keyName+"FlangeWidth",realFlangeWidth);
  Control.addVariable(keyName+"FlangeHeight",realFlangeHeight);
  Control.addVariable(keyName+"FlangeLength",flangeLen);

  Control.addVariable(keyName+"WindowActive",3);
  Control.addVariable(keyName+"WindowWidth",realWindowWidth);
  Control.addVariable(keyName+"WindowHeight",realWindowHeight);
  Control.addVariable(keyName+"WindowLength",windowThick);

  Control.addVariable(keyName+"FeMat",pipeMat);


  Control.addVariable(keyName+"WindowMat",windowMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  
  return;

}

}  // NAMESPACE setVariable
