/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/PitGenerator.cxx
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
#include "PitGenerator.h"

namespace setVariable
{

PitGenerator::PitGenerator() :
  feHeight(40.0),feDepth(feHeight),feWidth(feHeight),
  feFront(feHeight),feBack(feHeight),
  concHeight(40.0),concDepth(concHeight),concWidth(concHeight),
  concFront(concHeight),concBack(concHeight),
  feMat("CastIron"),concMat("Concrete"),
  colletMat("Tungsten")
  /*!
    Constructor and defaults
  */
{}

PitGenerator::PitGenerator(const PitGenerator& A) : 
  feHeight(A.feHeight),feDepth(A.feDepth),feWidth(A.feWidth),
  feFront(A.feFront),feBack(A.feBack),concHeight(A.concHeight),
  concDepth(A.concDepth),concWidth(A.concWidth),
  concFront(A.concFront),concBack(A.concBack),feMat(A.feMat),
  concMat(A.concMat),colletMat(A.colletMat)
  /*!
    Copy constructor
    \param A :: PitGenerator to copy
  */
{}

PitGenerator&
PitGenerator::operator=(const PitGenerator& A)
  /*!
    Assignment operator
    \param A :: PitGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      feHeight=A.feHeight;
      feDepth=A.feDepth;
      feWidth=A.feWidth;
      feFront=A.feFront;
      feBack=A.feBack;
      concHeight=A.concHeight;
      concDepth=A.concDepth;
      concWidth=A.concWidth;
      concFront=A.concFront;
      concBack=A.concBack;
      feMat=A.feMat;
      concMat=A.concMat;
      colletMat=A.colletMat;
    }
  return *this;
}

  
PitGenerator::~PitGenerator() 
 /*!
   Destructor
 */
{}


void
PitGenerator::setFeLayer(const double T)
  /*!
    Set all the Fe layers to the same value
    \param T :: Thickness
   */
{
  feHeight=T;
  feDepth=T;
  feWidth=T;
  feFront=T;
  feBack=T;

  return;
}

void
PitGenerator::setConcLayer(const double T)
  /*!
    Set all the concrete layers to the same value
    \param T :: Thickness
   */
{
  concHeight=T;
  concDepth=T;
  concWidth=T;
  concFront=T;
  concBack=T;
  return;
}
  
void
PitGenerator::generatePit(FuncDataBase& Control,const std::string& keyName,
			  const double yStep,const double vLength,
                          const double vWidth,const double vHeight,
                          const double vDepth) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param vLength :: length of void
    \param vWidth :: full extent of void at sides [2x half]
    \param vHeight :: Full void height 
    \param vDepth :: Full void depth
  */
{
  ELog::RegMethod RegA("PitGenerator","generatorPit");


  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"VoidHeight",vHeight);
  Control.addVariable(keyName+"VoidDepth",vDepth);
  Control.addVariable(keyName+"VoidWidth",vWidth);
  Control.addVariable(keyName+"VoidLength",vLength);
  
  Control.addVariable(keyName+"FeHeight",feHeight);
  Control.addVariable(keyName+"FeDepth",feDepth);
  Control.addVariable(keyName+"FeWidth",feWidth);
  Control.addVariable(keyName+"FeFront",feFront);
  Control.addVariable(keyName+"FeBack",feBack);
  Control.addVariable(keyName+"FeMat",feMat);
  
  Control.addVariable(keyName+"ConcHeight",concHeight);
  Control.addVariable(keyName+"ConcDepth",concDepth);
  Control.addVariable(keyName+"ConcWidth",concWidth);
  Control.addVariable(keyName+"ConcFront",concFront);
  Control.addVariable(keyName+"ConcBack",concBack);
  Control.addVariable(keyName+"ConcMat",concMat);

  Control.addVariable(keyName+"ColletHeight",15.0);
  Control.addVariable(keyName+"ColletDepth",5.0);
  Control.addVariable(keyName+"ColletWidth",40.0);
  Control.addVariable(keyName+"ColletMat",colletMat);

  
  return;

}

}  // NAMESPACE setVariable
