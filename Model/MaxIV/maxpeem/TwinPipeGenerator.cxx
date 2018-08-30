/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/TwinPipeGenerator.cxx
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
#include "TwinPipeGenerator.h"

namespace setVariable
{

TwinPipeGenerator::TwinPipeGenerator() :
  pipeAXStep(0.0),pipeAZStep(0.0),
  pipeAXYAngle(15.0),pipeAZAngle(0.0),
  pipeARadius(3.2),pipeAThick(0.5),
  pipeBXStep(0.0),pipeBZStep(0.0),
  pipeBXYAngle(15.0),pipeBZAngle(0.0),
  pipeBRadius(3.2),pipeBThick(0.5),
  flangeCJRadius(5.7),flangeCJLength(0.8),
  flangeARadius(5.7),flangeALength(0.8),
  flangeBRadius(5.7),flangeBLength(0.8),
  pipeMat("Stainless304"),voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

TwinPipeGenerator::~TwinPipeGenerator() 
 /*!
   Destructor
 */
{}

void
TwinPipeGenerator::setAPos(const double XX,const double ZZ)
  /*!
    Set pipe x,z position
    \param XX :: X position
    \param ZZ :: Z position
  */
{
  pipeAXStep=XX;
  pipeAZStep=ZZ;
  return;
}

void
TwinPipeGenerator::setBPos(const double XX,const double ZZ)
  /*!
    Set pipe x,z position
    \param XX :: X position
    \param ZZ :: Z position
  */
{
  pipeBXStep=XX;
  pipeBZStep=ZZ;
  return;
}

void
TwinPipeGenerator::setXYAngle(const double A,const double B)
  /*!
    Set pipe/flange to CF-X format
    \param A :: Pipe A angle [deg]
    \param B :: Pipe B angle [deg]
  */
{
  pipeAXYAngle=A;
  pipeBXYAngle=B;
  return;
}
  
template<typename CF>
void
TwinPipeGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","setCF");

  setAPipeCF<CF>();
  setBPipeCF<CF>();

  return;
}

template<typename CF>
void
TwinPipeGenerator::setAPipeCF()
  /*!
    Set pipe A to CF-X format
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","setAPipeCF");

  pipeARadius=CF::innerRadius;
  pipeAThick=CF::wallThick;

  setAFlangeCF<CF>();

  return;
}
  
template<typename CF>
void
TwinPipeGenerator::setBPipeCF()
  /*!
    Set pipe B to CF-X format
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","setBPipeCF");

  pipeBRadius=CF::innerRadius;
  pipeBThick=CF::wallThick;

  setBFlangeCF<CF>();

  return;
}

template<typename CF>
void
TwinPipeGenerator::setJoinFlangeCF()
  /*!
    Set Join Flnage to CF-X format
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","setJoinFlangeCF");

  flangeCJRadius=CF::flangeRadius;
  flangeCJLength=CF::flangeLength;

  return;
}

template<typename CF>
void
TwinPipeGenerator::setAFlangeCF()
  /*!
    Set pipe B to CF-X format
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","setAFlangeCF");

  flangeARadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;

  return;
}

template<typename CF>
void
TwinPipeGenerator::setBFlangeCF()
  /*!
    Set pipe B to CF-X format
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","setBFlangeCF");

  flangeBRadius=CF::flangeRadius;
  flangeBLength=CF::flangeLength;

  return;
}



void
TwinPipeGenerator::setMat(const std::string& PMat)
  /*!
    Set the material and the bellow material as a fractional
    void material
    \param PMat :: pipe material
  */
{
  pipeMat=PMat;
  return;
}
  
void
TwinPipeGenerator::generateTwin(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double FLen) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param JLen :: length of joined pipe
    \param FLen :: full length of pipes
  */
{
  ELog::RegMethod RegA("TwinPipeGenerator","generatorTwin");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"PipeAXStep",pipeAXStep);
  Control.addVariable(keyName+"PipeAZStep",pipeAZStep);
  Control.addVariable(keyName+"PipeAXYAngle",pipeAXYAngle);
  Control.addVariable(keyName+"PipeAZAngle",pipeAZAngle);
  Control.addVariable(keyName+"PipeARadius",pipeARadius);
  Control.addVariable(keyName+"PipeALength",FLen);
  Control.addVariable(keyName+"PipeAThick",pipeAThick);

  Control.addVariable(keyName+"PipeBXStep",pipeBXStep);
  Control.addVariable(keyName+"PipeBZStep",pipeBZStep);
  Control.addVariable(keyName+"PipeBXYAngle",pipeBXYAngle);
  Control.addVariable(keyName+"PipeBZAngle",pipeBZAngle);
  Control.addVariable(keyName+"PipeBRadius",pipeBRadius);
  Control.addVariable(keyName+"PipeBLength",FLen);
  Control.addVariable(keyName+"PipeBThick",pipeBThick);

  Control.addVariable(keyName+"FlangeCJRadius",flangeCJRadius);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeBRadius",flangeBRadius);

  Control.addVariable(keyName+"FlangeCJLength",flangeCJLength);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);

  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"FeMat",pipeMat);

  return;

}


///\cond TEMPLATE

  template void TwinPipeGenerator::setCF<CF40>();
  template void TwinPipeGenerator::setCF<CF63>();
  template void TwinPipeGenerator::setCF<CF100>();
  template void TwinPipeGenerator::setCF<CF150>();

  template void TwinPipeGenerator::setAPipeCF<CF40>();
  template void TwinPipeGenerator::setAPipeCF<CF63>();
  template void TwinPipeGenerator::setAPipeCF<CF100>();
  template void TwinPipeGenerator::setAPipeCF<CF150>();

  template void TwinPipeGenerator::setBPipeCF<CF40>();
  template void TwinPipeGenerator::setBPipeCF<CF63>();
  template void TwinPipeGenerator::setBPipeCF<CF100>();
  template void TwinPipeGenerator::setBPipeCF<CF150>();

  template void TwinPipeGenerator::setJoinFlangeCF<CF40>();
  template void TwinPipeGenerator::setJoinFlangeCF<CF63>();
  template void TwinPipeGenerator::setJoinFlangeCF<CF100>();
  template void TwinPipeGenerator::setJoinFlangeCF<CF150>();

  template void TwinPipeGenerator::setAFlangeCF<CF40>();
  template void TwinPipeGenerator::setAFlangeCF<CF63>();
  template void TwinPipeGenerator::setAFlangeCF<CF100>();
  template void TwinPipeGenerator::setAFlangeCF<CF150>();

  template void TwinPipeGenerator::setBFlangeCF<CF40>();
  template void TwinPipeGenerator::setBFlangeCF<CF63>();
  template void TwinPipeGenerator::setBFlangeCF<CF100>();
  template void TwinPipeGenerator::setBFlangeCF<CF150>();

///\endcond TEMPLATE


  
}  // NAMESPACE setVariable
