/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/PipeGenerator.cxx
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
#include "PipeGenerator.h"

namespace setVariable
{

PipeGenerator::PipeGenerator() :
  pipeType(0),pipeRadius(8.0),
  pipeHeight(16.0),pipeWidth(16.0),pipeThick(0.5),
  flangeRadius(12.0),flangeLen(1.0),
  windowRadius(10.0),windowThick(0.5),
  pipeMat("Aluminium"),windowMat("Silicon300K"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

  
PipeGenerator::~PipeGenerator() 
 /*!
   Destructor
 */
{}


void
PipeGenerator::setPipe(const double R,const double T)
  /*!
    Set all the pipe values
    \param R :: radius of main pipe
    \param T :: Thickness
   */
{
  pipeType=0;
  pipeRadius=R;
  pipeThick=T;
  return;
}

void
PipeGenerator::setRectPipe(const double W,const double H,
                           const double T)
  /*!
    Set all the pipe values
    \param W :: full width of main pipe
    \param H :: full height of main pipe
    \param T :: Thickness
   */
{
  pipeType=1;
  pipeWidth=W;
  pipeHeight=H;
  pipeThick=T;
  return;
}

void
PipeGenerator::setWindow(const double R,const double T)
  /*!
    Set all the window values
    \param R :: radius of window
    \param T :: Thickness
   */
{
  windowRadius=R;
  windowThick=T;
  return;
}


void
PipeGenerator::setFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeRadius=R;
  flangeLen=L;
  return;
}
  
void
PipeGenerator::generatePipe(FuncDataBase& Control,const std::string& keyName,
                            const double yStep,const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("PipeGenerator","generatorPipe");

  const double minRadius(pipeType ?
                         std::min(pipeWidth,pipeHeight) :
                         pipeRadius);
  double realWindowRadius=(windowRadius<0.0) ?
    minRadius-windowRadius : windowRadius;
  const double realFlangeRadius=(flangeRadius<0.0) ?
    minRadius-flangeRadius : flangeRadius;
  realWindowRadius=std::min(realWindowRadius,realFlangeRadius);

    // VACUUM PIPES:
  Control.addVariable(keyName+"YStep",yStep);   // step + flange
  if (!pipeType)
    Control.addVariable(keyName+"Radius",pipeRadius);
  else
    {
      Control.addVariable(keyName+"Width",pipeWidth);
      Control.addVariable(keyName+"Height",pipeHeight);
    }
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FeThick",pipeThick);
  Control.addVariable(keyName+"FlangeRadius",realFlangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLen);
  Control.addVariable(keyName+"FeMat",pipeMat);
  Control.addVariable(keyName+"WindowActive",3);

  Control.addVariable(keyName+"WindowRadius",realWindowRadius);
  
  Control.addVariable(keyName+"WindowThick",windowThick);
  Control.addVariable(keyName+"WindowMat",windowMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  
  return;

}

}  // NAMESPACE setVariable
