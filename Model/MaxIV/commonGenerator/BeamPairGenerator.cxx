/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BeamPairGenerator.cxx
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

#include "BeamPairGenerator.h"

namespace setVariable
{

BeamPairGenerator::BeamPairGenerator() :
  outLiftA(7.0),outLiftB(5.0),
  gapA(0.3),gapB(0.3),supportRadius(0.5),
  supportMat("Nickel"),
  xStepA(0.4),yStepA(-0.8),
  xStepB(-0.4),yStepB(0.8),
  xyAngle(0.0),height(2.0),width(4.0),
  length(1.0),blockMat("Tungsten")
  /*!
    Constructor and defaults
  */
{}

BeamPairGenerator::~BeamPairGenerator() 
 /*!
   Destructor
 */
{}



void
BeamPairGenerator::setLift(const double A,const double B)
  /*!
    Set the thread 
    \param A :: beam/out lift
    \param B :: beam/out lift
   */
{
  outLiftA=A;
  outLiftB=B;
  return;
}

void
BeamPairGenerator::setGap(const double A,const double B)
  /*!
    Set the thread 
    \param A :: gap upward 
    \param B :: gap downwar
   */
{
  gapA=A;   // up directoin
  gapB=B;
  return;
}

void
BeamPairGenerator::setThread(const double R,
			     const std::string& Mat)
  /*!
    Set the thread 
    \param R :: Thread radius
    \param Mat :: Material of support
   */
{
  supportRadius=R;
  supportMat=Mat;
  return;
}

void
BeamPairGenerator::setBlock(const double W,const double H,
			    const double L,const double XYA,
			    const std::string& Mat)
/*!
    Set the centred block
    \param W :: Width
    \param H :: Height
    \param L :: Length
    \param XYA :: xyAngle
    \param Mat :: Material of blade
   */
{
  width=W;
  height=H;
  length=L;
  xyAngle=XYA;
  blockMat=Mat;
  return;
}

				  
void
BeamPairGenerator::generateMount(FuncDataBase& Control,
				  const std::string& keyName,
				  const int upFlag) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param blockType :: Block position [0:None,1:Mid,2:lower edge]		    \param upFlag :: true if item open/withdrawn
  */
{
  ELog::RegMethod RegA("BeamPairGenerator","generatorMount");

  Control.addVariable(keyName+"UpFlag",upFlag);

  Control.addVariable(keyName+"BeamZStep",0.0);

  Control.addVariable(keyName+"OutLiftA",outLiftA);
  Control.addVariable(keyName+"OutLiftB",outLiftB);
  Control.addVariable(keyName+"GapA",gapA);
  Control.addVariable(keyName+"GapB",gapB);

  Control.addVariable(keyName+"SupportRadius",supportRadius);
  Control.addVariable(keyName+"SupportMat",supportMat);

  Control.addVariable(keyName+"XStepA",xStepA);
  Control.addVariable(keyName+"YStepA",yStepA);
  Control.addVariable(keyName+"XStepB",xStepB);
  Control.addVariable(keyName+"YStepB",yStepB);
  
  // always add even if not needed
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Length",length);	
  Control.addVariable(keyName+"BlockMat",blockMat);

       
  return;

}

  
}  // NAMESPACE setVariable
