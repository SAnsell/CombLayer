/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/LeadPipeGenerator.cxx
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
#include "SplitPipeGenerator.h"
#include "LeadPipeGenerator.h"

namespace setVariable
{

LeadPipeGenerator::LeadPipeGenerator() :
  SplitPipeGenerator(),
  cladStep(1.0),cladThick(1.0),
  cladMat("Lead")
  /*!
    Constructor and defaults
  */
{}

LeadPipeGenerator::~LeadPipeGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
LeadPipeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  SplitPipeGenerator::setCF<CF>();
  
  cladStep=CF::bellowStep;
  cladThick=CF::bellowThick;
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
  cladStep=S;
  cladThick=BT;
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
  SplitPipeGenerator::setPipe(R,T);
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
  
  SplitPipeGenerator::setMat(PMat);
  cladMat=CMat;
  return;
}
  
void
LeadPipeGenerator::generateCladPipe(FuncDataBase& Control,
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
  ELog::RegMethod RegA("LeadPipeGenerator","generatorCladPipe");

  SplitPipeGenerator::generatePipe(Control,keyName,yStep,length);
  
  // VACUUM PIPES:
  Control.addVariable(keyName+"CladThick",cladThick);
  Control.addVariable(keyName+"CladStep",cladStep);
  Control.addVariable(keyName+"CladMat",cladMat);
      
  return;

}

///\cond TEMPLATE

template void LeadPipeGenerator::setCF<CF40>();
template void LeadPipeGenerator::setCF<CF63>();
template void LeadPipeGenerator::setCF<CF100>();
  
///\end TEMPLATE

  
  
}  // NAMESPACE setVariable
