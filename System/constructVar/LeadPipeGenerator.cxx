/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/LeadPipeGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "PipeGenerator.h"
#include "LeadPipeGenerator.h"

namespace setVariable
{

LeadPipeGenerator::LeadPipeGenerator() :
  PipeGenerator(),
  claddingThick(1.0),claddingStep(1.0),
  claddingMat("Lead")
  /*
    Constructor and defaults
  */
{}

template<typename CF>
void
LeadPipeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  ELog::RegMethod RegA("LeadPipeGenerator","setCF");
  PipeGenerator::setCF<CF>();

  claddingStep=CF::bellowStep;
  claddingThick=CF::bellowThick;
  return;
}

void
LeadPipeGenerator::setCladding(const double S,const double BT)
  /*!
    Cladding
    \param S :: Clad step
    \param BT :: Clad Thickness
  */
{
  claddingStep=S;
  claddingThick=BT;
  return;
}  
  
void
LeadPipeGenerator::setPipe(const double R,const double T,
			   const double S,const double BT)
  /*!
    Set all the pipe values
    \param R :: radius of main pipe
    \param T :: Thickness
    \param S :: Clad step
    \param BT :: Clad Thickness
   */
{
  PipeGenerator::setPipe(R,T);
  setCladding(S,BT);
  return;
}


void
LeadPipeGenerator::setMat(const std::string& PMat,
			  const std::string& CMat)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param PMat :: pipe material
    \param CMat :: cladding
  */
{
  ELog::RegMethod RegA("LeadPipeGenerator","setMat");
  
  PipeGenerator::setMat(PMat);
  claddingMat=CMat;
  return;
}
  
void
LeadPipeGenerator::generatePipe(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: length of pipe
  */
{
  ELog::RegMethod RegA("LeadPipeGenerator","generatorCladPipe");

  PipeGenerator::generatePipe(Control,keyName,length);
  // VACUUM PIPES:
  Control.addVariable(keyName+"CladdingThick",claddingThick);
  Control.addVariable(keyName+"CladdingStep",claddingStep);
  Control.addVariable(keyName+"CladdingMat",claddingMat);
  
  return;

}

///\cond TEMPLATE

template void LeadPipeGenerator::setCF<CF40>();
template void LeadPipeGenerator::setCF<CF63>();
template void LeadPipeGenerator::setCF<CF100>();


///\endcond TEMPLATE

  
  
}  // NAMESPACE setVariable
