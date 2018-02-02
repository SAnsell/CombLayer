/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/BellowGenerator.cxx
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
#include "BellowGenerator.h"

namespace setVariable
{

BellowGenerator::BellowGenerator() :
  pipeRadius(8.0),pipeThick(0.5),
  flangeARadius(12.0),flangeALen(1.0),
  flangeBRadius(12.0),flangeBLen(1.0),
  bellowStep(1.0),bellowThick(1.0),
  pipeMat("Stainless304"),voidMat("Void"),
  bellowMat("Stainless304%Void%50")
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
BellowGenerator::setAFlangeCF()
  /*!
    Setter for flange A
   */
{
  flangeARadius=CF::flangeRadius;
  flangeALen=CF::flangeLength;
  return;
}

template<typename CF>
void
BellowGenerator::setBFlangeCF()
  /*!
    Setter for flange B
   */
{
  flangeBRadius=CF::flangeRadius;
  flangeBLen=CF::flangeLength;
  return;
}

template<typename CF>
void
BellowGenerator::setCF()
  /*!
    Set pipe/flange to CF-X format
  */
{
  pipeRadius=CF::innerRadius;
  pipeThick=CF::wallThick;
  setAFlangeCF<CF>();
  setBFlangeCF<CF>();
  
  bellowStep=CF::bellowStep;
  bellowThick=CF::bellowThick;
  setMat(pipeMat,CF::bellowThick/CF::wallThick);
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
  pipeRadius=R;
  pipeThick=T;
  bellowStep=S;
  bellowThick=BT;
  return;
}


void
BellowGenerator::setFlange(const double R,const double L)
  /*!
    Set all the flange values
    \param R :: radius of flange
    \param L :: length
   */
{
  flangeARadius=R;
  flangeALen=L;
  flangeBRadius=R;
  flangeBLen=L;
  return;
}
  
void
BellowGenerator::setFlangePair(const double AR,const double AL,
			       const double BR,const double BL)
  /*!
    Set all the flange values
    \param AR :: radius of front flange
    \param AL :: front flange length
    \param BR :: radius of back flange
    \param BL :: back flange length
   */
{
  flangeARadius=AR;
  flangeALen=AL;
  flangeBRadius=BR;
  flangeBLen=BL;

  return;
}

void
BellowGenerator::setMat(const std::string& M,const double T)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param M :: Main material
    \param T :: Fractional materila
  */
{
  pipeMat=M;
  bellowMat=M+"%Void%"+std::to_string(T);
  return;
}
  
void
BellowGenerator::generateBellow(FuncDataBase& Control,
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
  ELog::RegMethod RegA("BellowGenerator","generatorBellow");


  const double realFlangeARadius=(flangeARadius<0.0) ?
    pipeRadius-flangeARadius : flangeARadius;
  const double realFlangeBRadius=(flangeBRadius<0.0) ?
    pipeRadius-flangeBRadius : flangeBRadius;

    // VACUUM PIPES:
  Control.addVariable(keyName+"YStep",yStep);   // step + flange
  Control.addVariable(keyName+"Radius",pipeRadius);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"FeThick",pipeThick);
  Control.addVariable(keyName+"BellowThick",bellowThick);

  Control.addVariable(keyName+"FlangeFrontRadius",realFlangeARadius);
  Control.addVariable(keyName+"FlangeBackRadius",realFlangeBRadius);
  Control.addVariable(keyName+"FlangeFrontLength",flangeALen);
  Control.addVariable(keyName+"FlangeBackLength",flangeBLen);
  Control.addVariable(keyName+"BellowStep",bellowStep);
  Control.addVariable(keyName+"FeMat",pipeMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"BellowMat",bellowMat);
      
  return;

}

///\cond TEMPLATE
  template void BellowGenerator::setCF<CF40>();
  template void BellowGenerator::setCF<CF63>();
  template void BellowGenerator::setCF<CF100>();
  template void BellowGenerator::setAFlangeCF<CF40>();
  template void BellowGenerator::setAFlangeCF<CF63>();
  template void BellowGenerator::setAFlangeCF<CF100>();
  template void BellowGenerator::setBFlangeCF<CF40>();
  template void BellowGenerator::setBFlangeCF<CF63>();
  template void BellowGenerator::setBFlangeCF<CF100>();
  
///\end TEMPLATE
  
}  // NAMESPACE setVariable
