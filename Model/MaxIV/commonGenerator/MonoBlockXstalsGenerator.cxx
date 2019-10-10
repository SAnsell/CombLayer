/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MonoBlockXstalsGenerator.cxx
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

#include "MonoBlockXstalsGenerator.h"

namespace setVariable
{

MonoBlockXstalsGenerator::MonoBlockXstalsGenerator() :
  gap(0.7),phiA(0.0),phiB(0.0),
  widthA(5.0),heightA(2.0),lengthA(5.0),
  widthB(3.0),heightB(2.0),lengthB(10.0),
  baseALength(11.0),baseAHeight(1.5),baseAWidth(6.0),
  topALength(9.0),topAHeight(1.5),topAWidth(5.0),
  baseBLength(14.0),baseBHeight(1.5),baseBWidth(6.0),
  topBLength(12.5),topBHeight(1.5),topBWidth(3.0),
  xtalMat("Silicon300K"),baseMat("Copper")
  /*!
    Constructor and defaults
  */
{}

MonoBlockXstalsGenerator::MonoBlockXstalsGenerator
(const MonoBlockXstalsGenerator& A) : 
  gap(A.gap),phiA(A.phiA),phiB(A.phiB),widthA(A.widthA),
  heightA(A.heightA),lengthA(A.lengthA),widthB(A.widthB),
  heightB(A.heightB),lengthB(A.lengthB),baseALength(A.baseALength),
  baseAHeight(A.baseAHeight),baseAWidth(A.baseAWidth),
  topALength(A.topALength),topAHeight(A.topAHeight),
  topAWidth(A.topAWidth),baseBLength(A.baseBLength),
  baseBHeight(A.baseBHeight),baseBWidth(A.baseBWidth),
  topBLength(A.topBLength),topBHeight(A.topBHeight),
  topBWidth(A.topBWidth),xtalMat(A.xtalMat),baseMat(A.baseMat)
  /*!
    Copy constructor
    \param A :: MonoBlockXstalsGenerator to copy
  */
{}

MonoBlockXstalsGenerator&
MonoBlockXstalsGenerator::operator=(const MonoBlockXstalsGenerator& A)
  /*!
    Assignment operator
    \param A :: MonoBlockXstalsGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      gap=A.gap;
      phiA=A.phiA;
      phiB=A.phiB;
      widthA=A.widthA;
      heightA=A.heightA;
      lengthA=A.lengthA;
      widthB=A.widthB;
      heightB=A.heightB;
      lengthB=A.lengthB;
      baseALength=A.baseALength;
      baseAHeight=A.baseAHeight;
      baseAWidth=A.baseAWidth;
      topALength=A.topALength;
      topAHeight=A.topAHeight;
      topAWidth=A.topAWidth;
      baseBLength=A.baseBLength;
      baseBHeight=A.baseBHeight;
      baseBWidth=A.baseBWidth;
      topBLength=A.topBLength;
      topBHeight=A.topBHeight;
      topBWidth=A.topBWidth;
      xtalMat=A.xtalMat;
      baseMat=A.baseMat;
    }
  return *this;
}


  
MonoBlockXstalsGenerator::~MonoBlockXstalsGenerator() 
 /*!
   Destructor
 */
{}
  

void
MonoBlockXstalsGenerator::generateXstal
(FuncDataBase& Control,const std::string& keyName,
 const double yStep,const double theta) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param theta :: Theta angle
  */
{
  ELog::RegMethod RegA("MonoBlockXstalsGenerator","generateXstal");
  
  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Gap",gap);
  Control.addVariable(keyName+"Theta",theta);

  Control.addVariable(keyName+"PhiA",phiA);
  Control.addVariable(keyName+"PhiB",phiB);

  Control.addVariable(keyName+"WidthA",widthA);
  Control.addVariable(keyName+"HeightA",heightA);
  Control.addVariable(keyName+"LengthA",lengthA);

  Control.addVariable(keyName+"WidthB",widthB);
  Control.addVariable(keyName+"HeightB",heightB);
  Control.addVariable(keyName+"LengthB",lengthB);

  Control.addVariable(keyName+"BaseALength",baseALength);
  Control.addVariable(keyName+"BaseAHeight",baseAHeight);
  Control.addVariable(keyName+"BaseAWidth",baseAWidth);

  Control.addVariable(keyName+"TopALength",topALength);
  Control.addVariable(keyName+"TopAHeight",topAHeight);
  Control.addVariable(keyName+"TopAWidth",topAWidth);

  Control.addVariable(keyName+"BaseBLength",baseBLength);
  Control.addVariable(keyName+"BaseBHeight",baseBHeight);
  Control.addVariable(keyName+"BaseBWidth",baseBWidth);

  Control.addVariable(keyName+"TopBLength",topBLength);
  Control.addVariable(keyName+"TopBHeight",topBHeight);
  Control.addVariable(keyName+"TopBWidth",topBWidth);

  Control.addVariable(keyName+"XtalMat",xtalMat);
  Control.addVariable(keyName+"BaseMat",baseMat);
  



    
  return;

}
  
}  // NAMESPACE setVariable
