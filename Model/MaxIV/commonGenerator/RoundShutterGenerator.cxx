/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/RoundShutterGenerator.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "PortTubeGenerator.h"
#include "SixPortGenerator.h"
#include "PortItemGenerator.h"
#include "ShutterUnitGenerator.h"
#include "RoundShutterGenerator.h"

namespace setVariable
{

RoundShutterGenerator::RoundShutterGenerator() :
  ITubeGen(new SixPortGenerator()),
  PItemGen(new PortItemGenerator()),
  SUnitGen(new ShutterUnitGenerator())
  /*!
    Constructor and defaults
  */
{
  ITubeGen->setCF<CF40>();
  ITubeGen->setSideCF<CF200>();
  ITubeGen->setSideLength(30.0);
  
  SUnitGen->setCF<CF40>();
  SUnitGen->setTopCF<CF40>();

  PItemGen->setCF<setVariable::CF40>(0.45);
  PItemGen->setPlate(0.0,"Void");  

  //  ITubeGen->setCF<CF::150>();
  // ITubeGen->setPipe(7.5,0.5);
  // ITubeGen->setPortCF<CF63>();
  // ITubeGen->setPortLength(3.0,3.0);

  //PTubeGen->setAPortOffset(0,-3.0);
  //  PTubeGen->setBPortOffset(0,-3.0);
 
}


RoundShutterGenerator::~RoundShutterGenerator() 
 /*!
   Destructor
 */
{}

void
RoundShutterGenerator::setMat(const std::string& M,const double T)
  /*!
    Set teh material and the bellow material as a fractional
    void material
    \param M :: Main material
    \param T :: Percentage of material
  */
{
  SUnitGen->setMat(M,T);
  return;
}
 
void
RoundShutterGenerator::generateShutter(FuncDataBase& Control,
				       const std::string& keyName,
				       const bool upFlagA,
				       const bool upFlagB) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param upFlagA :: First Shutter up
    \param upFlagB :: Second Shutter up
  */
{
  ELog::RegMethod RegA("RoundShutterGenerator","generateShutter");

  const double flangeHeight(14.0);  // beam centre to flange top (10+4cm)

  ITubeGen->setXSideLength(34,10);
  ITubeGen->generateSixPort(Control,keyName+"Pipe");


  SUnitGen->setThreadLength(flangeHeight+8.0);
  SUnitGen->setLift(0.0,4.0);
  SUnitGen->generateShutter(Control,keyName+"UnitA",upFlagA);
  SUnitGen->generateShutter(Control,keyName+"UnitB",upFlagB);
  Control.addVariable(keyName+"UnitABeamYAngle",90.0);
  Control.addVariable(keyName+"UnitBBeamYAngle",90.0);
    
  const Geometry::Vec3D ZVec(1,0,0);
  PItemGen->setLength(flangeHeight);
  PItemGen->generatePort(Control,keyName+"PortA",
			 Geometry::Vec3D(0,-4.5,0),ZVec);
  PItemGen->generatePort(Control,keyName+"PortB",
			 Geometry::Vec3D(0,4.5,0),ZVec);


  Control.addVariable(keyName+"DivideStep",7.8);
  Control.addVariable(keyName+"DivideThick",1.0);
  Control.addVariable(keyName+"DivideRadius",2.0);
  Control.addVariable(keyName+"DivideMat","Tungsten");
       
  return;
}

  
}  // NAMESPACE setVariable
