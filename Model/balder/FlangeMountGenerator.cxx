/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/FlangeMountGenerator.cxx
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "FlangeMountGenerator.h"

namespace setVariable
{

FlangeMountGenerator::FlangeMountGenerator() :
  plateRadius(7.0),plateThick(1.0),threadRadius(0.5),
  threadLength(20.0),bladeLift(10.0),bladeThick(1.0),
  bladeWidth(3.0),bladeHeight(4.0),threadMat("Nickel"),
  bladeMat("Tungsten"),plateMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

FlangeMountGenerator::~FlangeMountGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
FlangeMountGenerator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  plateRadius=CF::flangeRadius;
  plateThick=CF::flangeLength;
  return;
}

void
FlangeMountGenerator::setPlate(const double R,const double T,
			       const std::string& Mat)
  /*!
    Set the blade
    \param R :: Radius of flange plate
    \param T :: Thickness of flange plate
    \param Mat :: Material for flange
   */
{
  plateRadius=R;
  plateThick=T;
  plateMat=Mat;
  return;
}


void
FlangeMountGenerator::setThread(const double R,const double L,
				const std::string& Mat)
  /*!
    Set the threda
    \param R :: Thread radius
    \param L :: Thread length [to sample mid point]
    \param Mat :: Material of blade
   */
{
  threadRadius=R;
  threadLength=L;
  threadMat=Mat;
  return;
}

void
FlangeMountGenerator::setBlade(const double W,const double H,
			       const double T,const double Ang,
			       const std::string& Mat)
  /*!
    Set the blade
    \param H :: heigh of bea
    \param W :: Width of blade (across beam)
    \param T :: Thickness of blade
    \param Ang :: Angle of blade
    \param Mat :: Material of blade
   */
{
  bladeWidth=W;
  bladeHeight=H;
  bladeThick=T;
  bladeXYAngle=Ang;
  bladeMat=Mat;
  return;
}

void
FlangeMountGenerator::setMaterial(const std::string& PMat,
				  const std::string& TMat,
				  const std::string& BMat)
  /*!
    Set the materials
    \param PMat :: Plate Material
    \param TMat :: thread Material
    \param BMat :: Blade Material
  */
{
  plateMat=PMat;
  threadMat=TMat;
  bladeMat=BMat;
  return;
}

				  
void
FlangeMountGenerator::generateMount(FuncDataBase& Control,
				  const std::string& keyName,
				  const int inBeam) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param inBeam :: true if valve closed
  */
{
  ELog::RegMethod RegA("FlangeMountGenerator","generatorMount");
  
  Control.addVariable(keyName+"PlateRadius",plateRadius);
  Control.addVariable(keyName+"PlateThick",plateThick);

  Control.addVariable(keyName+"ThreadRadius",threadRadius);	
  Control.addVariable(keyName+"ThreadLength",threadLength);

  Control.addVariable(keyName+"InBeam",inBeam);

  Control.addVariable(keyName+"BladeLift",bladeLift);
  Control.addVariable(keyName+"BladeXYAngle",bladeXYAngle);
  Control.addVariable(keyName+"BladeHeight",bladeHeight);
  Control.addVariable(keyName+"BladeThick",bladeThick);
  Control.addVariable(keyName+"BladeWidth",bladeWidth);

  Control.addVariable(keyName+"PlateMat",plateMat);
  Control.addVariable(keyName+"BladeMat",bladeMat);
  Control.addVariable(keyName+"ThreadMat",threadMat);
       
  return;

}

///\cond TEMPLATE

template void FlangeMountGenerator::setCF<CF40>();
template void FlangeMountGenerator::setCF<CF50>();
template void FlangeMountGenerator::setCF<CF63>();
template void FlangeMountGenerator::setCF<CF100>();

///\endcond  TEMPLATE
  
}  // NAMESPACE setVariable
