/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/SqrFMaskGenerator.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "CollGenerator.h"
#include "SqrFMaskGenerator.h"

namespace setVariable
{

SqrFMaskGenerator::SqrFMaskGenerator() :
  width(7.1),height(7.1),
  aFInRadius(4.0),aFOutRadius(6.0),
  aFLength(1.0),bFInRadius(4.0),
  bFOutRadius(6.0),bFLength(1.0),
  pipeRadius(0.3),pipeXWidth(4.0),
  pipeZDepth(1.8),
  pipeYStart(2.5),pipeYStep(3.0),
  flangeMat("Stainless304"),
  waterMat("H2O")
  /*!
    Constructor and defaults
  */
{}
  
SqrFMaskGenerator::~SqrFMaskGenerator() 
 /*!
   Destructor
 */
{}


template<typename CF>
void
SqrFMaskGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();
  
  return;
}

template<typename CF>
void
SqrFMaskGenerator::setAFlangeCF()
  /*!
    Setter for flange A
  */
{
  aFInRadius=CF::innerRadius;
  aFOutRadius=CF::flangeRadius;
  aFLength=CF::flangeLength;
  return;
}

template<typename CF>
void
SqrFMaskGenerator::setBFlangeCF()
  /*!
    Setter for flange B
  */
{
  bFInRadius=CF::innerRadius;
  bFOutRadius=CF::flangeRadius;
  bFLength=CF::flangeLength;
  return;
}
				  
void
SqrFMaskGenerator::generateColl(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double len) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param len :: length
  */
{
  ELog::RegMethod RegA("SqrFMaskGenerator","generateColl");

  CollGenerator::generateColl(Control,keyName,yStep,len);

  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"FlangeFrontInRadius",aFInRadius);
  Control.addVariable(keyName+"FlangeFrontOutRadius",aFOutRadius);
  Control.addVariable(keyName+"FlangeFrontLength",aFLength);
  
  Control.addVariable(keyName+"FlangeBackInRadius",bFInRadius);
  Control.addVariable(keyName+"FlangeBackOutRadius",bFOutRadius);
  Control.addVariable(keyName+"FlangeBackLength",bFLength);
  Control.addVariable(keyName+"FlangeMat",flangeMat);


  Control.addVariable(keyName+"PipeRadius",pipeRadius);
  Control.addVariable(keyName+"PipeXWidth",pipeXWidth);
  Control.addVariable(keyName+"PipeZDepth",pipeZDepth);
  Control.addVariable(keyName+"PipeYStep0",pipeYStart);
  Control.addVariable(keyName+"PipeYStep1",pipeYStart+pipeYStep);
  Control.addVariable(keyName+"PipeYStep2",pipeYStart+2*pipeYStep);
  Control.addVariable(keyName+"PipeYStep3",pipeYStart+3*pipeYStep);
  Control.addVariable(keyName+"WaterMat",waterMat);
  
  return;

}


///\cond TEMPLATE
  template void SqrFMaskGenerator::setCF<CF40>();
  template void SqrFMaskGenerator::setCF<CF63>();
  template void SqrFMaskGenerator::setCF<CF100>();
  template void SqrFMaskGenerator::setCF<CF120>();
  template void SqrFMaskGenerator::setAFlangeCF<CF40>();
  template void SqrFMaskGenerator::setAFlangeCF<CF63>();
  template void SqrFMaskGenerator::setAFlangeCF<CF100>();
  template void SqrFMaskGenerator::setAFlangeCF<CF120>();
  template void SqrFMaskGenerator::setBFlangeCF<CF40>();
  template void SqrFMaskGenerator::setBFlangeCF<CF63>();
  template void SqrFMaskGenerator::setBFlangeCF<CF100>();
  template void SqrFMaskGenerator::setBFlangeCF<CF120>();
  
///\endcond TEMPLATE
  
}  // NAMESPACE setVariable
