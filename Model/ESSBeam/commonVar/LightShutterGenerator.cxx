/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/LightShutterGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "LightShutterGenerator.h"

namespace setVariable
{

LightShutterGenerator::LightShutterGenerator() :
  length(50.0),width(30.0),height(30.0),wallThick(10.0),
  mainMat("Void"),wallMat("Aluminium")
  /*!
    Constructor and defaults
  */
{}

LightShutterGenerator::LightShutterGenerator(const LightShutterGenerator& A) : 
  length(A.length),width(A.width),height(A.height),wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: LightShutterGenerator to copy
  */
{}

LightShutterGenerator&
LightShutterGenerator::operator=(const LightShutterGenerator& A)
  /*!
    Assignment operator
    \param A :: LightShutterGenerator to copy
    \return *this
  */
{
  if (this!=&A)
  {
    length=A.length;
    width=A.width;
    height=A.height;
    wallThick=A.wallThick;
    mainMat=A.mainMat;
    wallMat=A.wallMat;
  }
  return *this;
}

LightShutterGenerator::~LightShutterGenerator() 
 /*!
   Destructor
 */
{}

void
LightShutterGenerator::setMaterials(const std::string& MM,const std::string& WM)
  /*!
    Set the materials
    \param MM :: Main material
    \param WM :: Wall material
  */
{
  ELog::RegMethod RegA("LightShutterGenerator","setMaterials");
  mainMat=MM;
  wallMat=WM;
  return;
}

void
LightShutterGenerator::setLength(const double L)
  /*!
    Set the length
    \param L :: Length
  */
{
  ELog::RegMethod RegA("LightShutterGenerator","setLength");
  length = L;
  return;
}

void
LightShutterGenerator::setWidth(const double W)
  /*!
    Set the width
    \param W :: Width
  */
{
  ELog::RegMethod RegA("LightShutterGenerator","setWidth");
  width = W;
  return;
}

void
LightShutterGenerator::setHeight(const double H)
  /*!
    Set the height
    \param H :: height
  */
{
  ELog::RegMethod RegA("LightShutterGenerator","setHeight");
  height = H;
  return;
}

void
LightShutterGenerator::setWallThick(const double T)
  /*!
    Set thickness
    \param T :: Wall thickness
   */
{
  ELog::RegMethod RegA("LightShutterGenerator","setWallThick");
  wallThick=T;
  if (wallThick<Geometry::zeroTol)
    ELog::EM<<"Failure: WallThick variable is too small"<<ELog::endErr;
  return;
}

void
LightShutterGenerator::setOpenPercentage(const double H,const double Z)
  /*!
    Set the zStep
    \param H :: The height of the section that has to be closed
    \param Z :: 
                Z =  0  :: zStep = 0 -> the centre of the LightShutter == H/2
                Z =  1  :: Light Shutter bottom edge = +H/2
                Z = -1  :: Light Shutter top edge = -H/2
                Z > 1 || Z < 1 :: Light Shutter bottom edge > +H/2 || Light Shutter top edge < -H/2
  */
{
  ELog::RegMethod RegA("LightShutterGenerator","setClosingPercentage");
  realZStep=Z*(0.5*H+0.5*height+wallThick);
  return;
}

void
LightShutterGenerator::generateLightShutter(FuncDataBase& Control,
                                            const std::string& keyName,
                                            const double yStep) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset
    \param length :: Length
    \param width :: Width
    \param height :: Height
  */
{
  ELog::RegMethod RegA("LightShutterGenerator","generatorLightShutter");
  
  Control.addVariable(keyName+"XStep",0.0);
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",realZStep!=0? realZStep : 0);
  Control.addVariable(keyName+"XYangle",0.0);
  Control.addVariable(keyName+"Zangle",0.0);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  
  Control.addVariable(keyName+"WallThick",wallThick);
  
  Control.addVariable(keyName+"MainMat",mainMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  
  if (length<Geometry::zeroTol)
    ELog::EM<<"Failure: length is too small"<<ELog::endErr;
  if (width<Geometry::zeroTol)
    ELog::EM<<"Failure: width is too small"<<ELog::endErr;
  if (height<Geometry::zeroTol)
    ELog::EM<<"Failure: height is too small"<<ELog::endErr;
  return;

}

}  // NAMESPACE setVariable
