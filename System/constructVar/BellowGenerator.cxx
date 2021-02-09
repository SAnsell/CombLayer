/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVar/BellowGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "CFFlanges.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"

namespace setVariable
{

BellowGenerator::BellowGenerator() :
  SplitPipeGenerator(),
  bellowStep(1.0),bellowThick(1.0),
  bellowMat("Stainless304%Void%10.0")
  /*!
    Constructor and defaults
  */
{}

BellowGenerator::~BellowGenerator()
 /*!
   Destructor
 */
{}

template<typename CF>
void
BellowGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  ELog::RegMethod RegA("BellowGenerator","setCF");
  SplitPipeGenerator::setCF<CF>();

  bellowStep=CF::bellowStep;
  bellowThick=CF::bellowThick;

  // this sets to 10% the materials fraction based on a 50/50 split
  setMat(SplitPipeGenerator::getPipeMat(),
	 20.0*CF::wallThick/CF::bellowThick);
  return;
}

void
BellowGenerator::setPipe(const double R,const double T,
			 const double S,const double BT)
  /*!
    Set all the pipe values
    \param R :: radius of main pipe
    \param T :: Thickness
    \parma S :: Bellow step
    \param BT :: Bellow Thickness
   */
{
  SplitPipeGenerator::setPipe(R,T);
  bellowStep=S;
  bellowThick=BT;
  return;
}


void
BellowGenerator::setMat(const std::string& M,const double T)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param M :: Main material
    \param T :: Percentage of material
  */
{
  SplitPipeGenerator::setMat(M);
  bellowMat=M+"%Void%"+std::to_string(T);
  return;
}

void
BellowGenerator::setMat(const std::string& PMat,
			const std::string& CMat)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param PMat :: pipe material
    \param CMat :: cladding
  */
{
  SplitPipeGenerator::setMat(PMat);
  bellowMat=CMat;
  return;
}

void
BellowGenerator::generateBellow(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: y-offset
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("BellowGenerator","generatorBellow");

  SplitPipeGenerator::generatePipe(Control,keyName,length);
  // VACUUM PIPES:
  Control.addVariable(keyName+"BellowThick",bellowThick);
  Control.addVariable(keyName+"BellowStep",bellowStep);
  Control.addVariable(keyName+"BellowMat",bellowMat);

  return;

}

///\cond TEMPLATE

  template void BellowGenerator::setCF<CF18_TDC>();
  template void BellowGenerator::setCF<CF26_TDC>();
  template void BellowGenerator::setCF<CF37_TDC>();
  template void BellowGenerator::setCF<CF40_22>();
  template void BellowGenerator::setCF<CF40>();
  template void BellowGenerator::setCF<CF50>();
  template void BellowGenerator::setCF<CF63>();
  template void BellowGenerator::setCF<CF100>();

///\endcond TEMPLATE



}  // NAMESPACE setVariable
