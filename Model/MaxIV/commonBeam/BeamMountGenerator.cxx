/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/BeamMountGenerator.cxx
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
#include "BeamMountGenerator.h"

namespace setVariable
{

BeamMountGenerator::BeamMountGenerator() :
  blockType(0),outLift(5.0),beamLift(0.0),supportRadius(1.0),
  supportMat("Nickel"),height(4.0),width(3.0),
  length(5.0),blockMat("Tungsten")
  /*!
    Constructor and defaults
  */
{}

BeamMountGenerator::~BeamMountGenerator() 
 /*!
   Destructor
 */
{}



void
BeamMountGenerator::setLift(const double A,
			    const double B)
  /*!
    Set the thread 
    \param A :: beam/out lift
    \param B :: beam/out lift
   */
{
  beamLift=std::min(A,B);
  outLift=std::max(A,B);
  return;
}

void
BeamMountGenerator::setThread(const double R,
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
BeamMountGenerator::setCentreBlock(const double W,const double H,
				   const double L,const std::string& Mat)
  /*!
    Set the centred block
    \param W :: Width
    \param H :: Height
    \param L :: Length
    \param Mat :: Material of blade
   */
{
  blockType=1;
  width=W;
  height=H;
  length=L;
  blockMat=Mat;
  return;
}
void
BeamMountGenerator::setEdgeBlock(const double W,const double H,
				 const double L,const std::string& Mat)
  /*!
    Set the edge  block
    \param W :: Width
    \param H :: Height
    \param L :: Length
    \param Mat :: Material of blade
   */
{
  blockType=2;
  width=W;
  height=H;
  length=L;
  blockMat=Mat;
  return;
}


				  
void
BeamMountGenerator::generateMount(FuncDataBase& Control,
				  const std::string& keyName,
				  const int upFlag) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param blockType :: Block position [0:None,1:Mid,2:lower edge]		    \param upFlag :: true if valve closed
  */
{
  ELog::RegMethod RegA("BeamMountGenerator","generatorMount");

  Control.addVariable(keyName+"BlockFlag",blockType);
  Control.addVariable(keyName+"UpFlag",upFlag);

  Control.addVariable(keyName+"OutLift",outLift);
  Control.addVariable(keyName+"BeamLift",beamLift);
  
  Control.addVariable(keyName+"SupportRadius",supportRadius);
  Control.addVariable(keyName+"SupportMat",supportMat);
  
  // always add even if not needed
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Length",length);	
  Control.addVariable(keyName+"BlockMat",blockMat);

       
  return;

}

  
}  // NAMESPACE setVariable
