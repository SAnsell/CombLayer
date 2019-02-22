/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/MonoShutterGenerator.cxx
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
#include "PortTubeGenerator.h"
#include "PortItemGenerator.h"
#include "ShutterUnitGenerator.h"
#include "MonoShutterGenerator.h"

namespace setVariable
{

MonoShutterGenerator::MonoShutterGenerator() :
  PTubeGen(new PortTubeGenerator()),
  PItemGen(new PortItemGenerator()),
  SUnitGen(new ShutterUnitGenerator())
  /*!
    Constructor and defaults
  */
{
  SUnitGen->setCF<CF40>();
  SUnitGen->setTopCF<CF40>();

  PItemGen->setCF<setVariable::CF40>(0.45);
  PItemGen->setPlate(0.0,"Void");  

  PTubeGen->setCF<CF63>();
  PTubeGen->setPortLength(3.0,3.0);

  PTubeGen->setAPortOffset(0,-3.0);
  PTubeGen->setBPortOffset(0,-3.0);
 
}


MonoShutterGenerator::~MonoShutterGenerator() 
 /*!
   Destructor
 */
{}

void
MonoShutterGenerator::setMat(const std::string& M,const double T)
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
MonoShutterGenerator::generateShutter(FuncDataBase& Control,
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
  ELog::RegMethod RegA("MonoShutterGenerator","generateShutter");
  
  SUnitGen->generateShutter(Control,keyName+"UnitA",upFlagA);
  SUnitGen->generateShutter(Control,keyName+"UnitB",upFlagB);
  
  // Main outer structure:
  PTubeGen->generateTube(Control,keyName+"Pipe",0.0,7.50,20.0);
  Control.addVariable(keyName+"PipeNPorts",2);
  
  const Geometry::Vec3D ZVec(0,0,1);
  PItemGen->generatePort(Control,keyName+"PipePort0",
			Geometry::Vec3D(0,-4,0),ZVec);
  PItemGen->generatePort(Control,keyName+"PipePort1",
			 Geometry::Vec3D(0,4,0),ZVec);


  Control.addVariable(keyName+"DivideBStep",7.8);
  Control.addVariable(keyName+"DivideThick",1.0);
  Control.addVariable(keyName+"DivideRadius",2.0);
  Control.addVariable(keyName+"DivideMat","Tungsten");
       
  return;
}

  
}  // NAMESPACE setVariable
