/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/BremBlockGenerator.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "BremBlockGenerator.h"

namespace setVariable
{

BremBlockGenerator::BremBlockGenerator() :
  centFlag(0),radius(0.0),width(7.2),height(7.2),
  holeXStep(0.0),holeZStep(0.0),
  holeAWidth(3.0),holeAHeight(1.5),
  holeMidDist(-0.6),holeMidWidth(0.7),holeMidHeight(0.7),
  holeBWidth(1.0),holeBHeight(1.0),
  
  voidMat("Void"),mainMat("Tungsten")

  /*!
    Constructor and defaults
  */
{}

BremBlockGenerator::~BremBlockGenerator() 
 /*!
   Destructor
 */
{}

  
void
BremBlockGenerator::setMaterial(const std::string& MMat,
				const std::string& VMat)
  /*!
    Set the materials
    \param MMat :: main Material
    \param VMat :: Void material
  */
{
  mainMat=MMat;
  voidMat=VMat;

  return;
}

void
BremBlockGenerator::setRadius(const double R)
  /*!
    Make block cylinder
    \param R :: Radius
   */
{
  radius=R;
  width=-1.0;
  height=-1.0;
  return;
}

void
BremBlockGenerator::setCube(const double W,const double H)
  /*!
    Make block blockshape
    \param W :: Width
    \param H :: Height
   */
{
  radius=-1.0;
  width=W;
  height=H;
  return;
}

void
BremBlockGenerator::setHoleXY(const double HX,const double HZ)
  /*!
    Set the hole offset
    \param HX :: X step
    \param HZ :: Z step
  */
{
  holeXStep=HX;
  holeZStep=HZ;
  return;
}


void
BremBlockGenerator::setAperature(const double MLength,
				 const double frontW,const double frontH,
				 const double midW,const double midH,
				 const double backW,const double backH)
/*!
    Set the widths
    \param MLength :: Aperature length
    \param frontW :: Front width
    \param frontH :: Front height
    \param midW :: min width
    \param midH :: min height
    \param backW :: back width
    \param backH :: back height
  */
{
  holeMidDist=MLength;
  holeAWidth=frontW;
  holeAHeight=frontH;
  holeMidWidth=midW;
  holeMidHeight=midH;
  holeBWidth=backW;
  holeBHeight=backH;
  return;
}

				  
void
BremBlockGenerator::generateBlock(FuncDataBase& Control,
				  const std::string& keyName,
				  const double yStep,
				  const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Forward step
    \param length :: length of W block
  */
{
  ELog::RegMethod RegA("BremBlockGenerator","generatorColl");
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"CentreFlag",static_cast<int>(centFlag));
  
  if (radius<Geometry::zeroTol)
    {
      Control.addVariable(keyName+"Width",width);
      Control.addVariable(keyName+"Height",height);
    }
  else
    Control.addVariable(keyName+"Radius",radius);
  
  Control.addVariable(keyName+"Length",length);

  const double HD(holeMidDist<0.0 ? -length*holeMidDist : holeMidDist);
  Control.addVariable(keyName+"HoleXStep",holeXStep);
  Control.addVariable(keyName+"HoleZStep",holeZStep);
  Control.addVariable(keyName+"HoleAWidth",holeAWidth);
  Control.addVariable(keyName+"HoleAHeight",holeAHeight);
  Control.addVariable(keyName+"HoleMidDist",HD);
  Control.addVariable(keyName+"HoleMidWidth",holeMidWidth);
  Control.addVariable(keyName+"HoleMidHeight",holeMidHeight);
  Control.addVariable(keyName+"HoleBWidth",holeBWidth);
  Control.addVariable(keyName+"HoleBHeight",holeBHeight);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"MainMat",mainMat);
       
  return;

}

  
}  // NAMESPACE setVariable
