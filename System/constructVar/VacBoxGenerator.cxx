/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/VacBoxGenerator.cxx
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
#include "VacBoxGenerator.h"

namespace setVariable
{

VacBoxGenerator::VacBoxGenerator() :
  wallThick(0.5),portWallThick(0.5),
  portTubeLength(5.0),portTubeRadius(4.0),
  flangeLen(1.0),flangeRadius(1.0),
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

VacBoxGenerator::VacBoxGenerator(const VacBoxGenerator& A) : 
  wallThick(A.wallThick),portWallThick(A.portWallThick),
  portTubeLength(A.portTubeLength),portTubeRadius(A.portTubeRadius),
  flangeLen(A.flangeLen),flangeRadius(A.flangeRadius),
  voidMat(A.voidMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: VacBoxGenerator to copy
  */
{}

VacBoxGenerator&
VacBoxGenerator::operator=(const VacBoxGenerator& A)
  /*!
    Assignment operator
    \param A :: VacBoxGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      wallThick=A.wallThick;
      portWallThick=A.portWallThick;
      portTubeLength=A.portTubeLength;
      portTubeRadius=A.portTubeRadius;
      flangeLen=A.flangeLen;
      flangeRadius=A.flangeRadius;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}
  
VacBoxGenerator::~VacBoxGenerator() 
 /*!
   Destructor
 */
{}


void
VacBoxGenerator::setPort(const double R,const double L,
			 const double T)
  /*!
    Set both the ports
    \param R :: radius of port tube
    \param R :: lenght of port tube
    \param T :: Thickness of port tube
   */
{
  portTubeRadius=R;
  portTubeLength=L;
  portWallThick=T;
  return;
}

void
VacBoxGenerator::setFlange(const double R,const double L)
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
VacBoxGenerator::generateBox(FuncDataBase& Control,const std::string& keyName,
			     const double yStep,const double width,const
			     double height,const double depth,
			     const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param height :: height of box
    \param depth :: depth of box
    \param width :: width of box (full)
    \param length :: length of box - ports
  */
{
  ELog::RegMethod RegA("VacBoxGenerator","generatorBox");
  

  Control.addVariable(keyName+"YStep",yStep);   // step + flange

  Control.addVariable(keyName+"VoidHeight",height);
  Control.addVariable(keyName+"VoidDepth",depth);
  Control.addVariable(keyName+"VoidWidth",width);
  Control.addVariable(keyName+"VoidLength",length);

  Control.addVariable(keyName+"WallThick",wallThick);
	
  Control.addVariable(keyName+"PortWallThick",portWallThick);
  Control.addVariable(keyName+"PortTubeRadius",portTubeRadius);
  Control.addVariable(keyName+"PortTubeLength",portTubeLength);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLen);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"FeMat",wallMat);
       
  return;

}

}  // NAMESPACE setVariable
