/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/BremCollGenerator.cxx
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

#include "CFFlanges.h"
#include "BremCollGenerator.h"

namespace setVariable
{

BremCollGenerator::BremCollGenerator() :
  width(7.2),height(6.6),wallThick(0.5),
  holeXStep(0.0),holeZStep(0.0),
  holeAWidth(3.0),holeAHeight(1.5),
  holeMidDist(-1.0),holeMidWidth(0.7),holeMidHeight(0.7),
  holeBWidth(1.0),holeBHeight(1.0),
  extLength(5.0),extRadius(2.5),pipeDepth(2.0),pipeXSec(0.9),
  pipeYStep(2.2),pipeZStep(2.0),pipeWidth(5.2),pipeMidGap(1.0),
  
  voidMat("Void"),innerMat("Tungsten"),
  wallMat("Stainless304"),waterMat("H2O"),
  pipeMat("Copper")
  /*!
    Constructor and defaults
  */
{
  setCF<CF40>();
}

BremCollGenerator::~BremCollGenerator() 
 /*!
   Destructor
 */
{}

template<typename CF>
void
BremCollGenerator::setCF()
  /*!
    Set pipe/flange to CF format
  */
{
  innerRadius=CF::innerRadius;
  flangeARadius=CF::flangeRadius;
  flangeBRadius=CF::flangeRadius;
  flangeALength=CF::flangeLength;
  flangeBLength=CF::flangeLength;
  return;
}
  
void
BremCollGenerator::setMaterial(const std::string& IMat,
			       const std::string& WMat)
  /*!
    Set the materials
    \param PMat :: Plate Material
    \param TMat :: thread Material
    \param BMat :: Blade Material
  */
{
  innerMat=IMat;
  wallMat=WMat;
  return;
}

void
BremCollGenerator::setAperature(const double frontW,const double frontH,
				const double midW,const double midH,
				const double backW,const double backH)
  /*!
    Set the widths
    \param frontW :: Front width
    \param frontH :: Front height
    \param midW :: min width
    \param midH :: min height
    \param backW :: back width
    \param backH :: back height
  */
{
  holeAWidth=frontW;
  holeAHeight=frontH;
  holeMidWidth=midW;
  holeMidHeight=midH;
  holeBWidth=backW;
  holeBHeight=backH;
  return;
}

				  
void
BremCollGenerator::generateColl(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Forward step
    \param length :: length of W block
  */
{
  ELog::RegMethod RegA("BremCollGenerator","generatorColl");
  
  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"WallThick",wallThick);


  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"FlangeARadius",flangeARadius);
  Control.addVariable(keyName+"FlangeALength",flangeALength);
  Control.addVariable(keyName+"FlangeBRadius",flangeARadius);
  Control.addVariable(keyName+"FlangeBLength",flangeBLength);  

  Control.addVariable(keyName+"HoleXStep",holeXStep);
  Control.addVariable(keyName+"HoleZStep",holeZStep);
  Control.addVariable(keyName+"HoleAWidth",holeAWidth);
  Control.addVariable(keyName+"HoleAHeight",holeAHeight);
  Control.addVariable(keyName+"HoleMidDist",holeMidDist);
  Control.addVariable(keyName+"HoleMidWidth",holeMidWidth);
  Control.addVariable(keyName+"HoleMidHeight",holeMidHeight);
  Control.addVariable(keyName+"HoleBWidth",holeBWidth);
  Control.addVariable(keyName+"HoleBHeight",holeBHeight);

  Control.addVariable(keyName+"ExtLength",extLength);
  Control.addVariable(keyName+"ExtRadius",extRadius);


  Control.addVariable(keyName+"PipeDepth",pipeDepth);
  Control.addVariable(keyName+"PipeXSec",pipeXSec);
  Control.addVariable(keyName+"PipeYStep",pipeYStep);
  Control.addVariable(keyName+"PipeZStep",pipeZStep);
  Control.addVariable(keyName+"PipeWidth",pipeWidth);
  Control.addVariable(keyName+"PipeMidGap",pipeMidGap);
  
  Control.addVariable(keyName+"InnerMat",innerMat);
  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WaterMat",waterMat);
  Control.addVariable(keyName+"PipeMat",pipeMat);
       
  return;

}

///\cond TEMPLATE

template void BremCollGenerator::setCF<CF40>();
template void BremCollGenerator::setCF<CF50>();
template void BremCollGenerator::setCF<CF63>();
template void BremCollGenerator::setCF<CF100>();

///\endcond  TEMPLATE
  
}  // NAMESPACE setVariable
