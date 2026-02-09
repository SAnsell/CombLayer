/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MonoShutterR3Generator.cxx
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "ShutterUnitGenerator.h"
#include "MonoShutterR3Generator.h"

namespace setVariable
{

MonoShutterR3Generator::MonoShutterR3Generator() :
  PTubeGen(new PipeTubeGenerator()),
  PItemGen(new PortItemGenerator()),
  SUnitGen(new ShutterUnitGenerator()),
  apertureBackLength(7.0),apertureInnerRadius(1.0),apertureMat("Tungsten"),
  apertureOuterRadius(5.15),
  apertureThick(1.0),apertureToBlockGap(0.4),
  blockHeight(5.0),blockLength(5.0),blockWidth(5.0),flangeThick(1.225),height(25.4),
  length(35.0),shutterDistance(7.1)
  /*!
    Constructor and defaults
  */
{}


MonoShutterR3Generator::~MonoShutterR3Generator() 
 /*!
   Destructor
 */
{}
 
void
MonoShutterR3Generator::generateShutter(FuncDataBase& Control,
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
  ELog::RegMethod RegA("MonoShutterR3Generator","generateShutter");

  SUnitGen->setCF<CF40>();
  SUnitGen->setBlock(blockHeight,blockLength,blockWidth);
  SUnitGen->generateShutter(Control,keyName+"UnitA",upFlagA);
  SUnitGen->generateShutter(Control,keyName+"UnitB",upFlagB);
  
  PTubeGen->setCF<CF200>();
  PTubeGen->setAFlange(CF200::flangeRadius,flangeThick);
  PTubeGen->setBFlange(CF200::flangeRadius,flangeThick);
  PTubeGen->setCap(1,1);
  PTubeGen->generateTube(Control,keyName+"Pipe",
    height-2.0*flangeThick);
  Control.addVariable(keyName+"PipeNPorts",4);
  
  const Geometry::Vec3D Y(0,1,0);
  const Geometry::Vec3D Z(0,0,1);
  PItemGen->setCF<CF63>(0.5*length);
  PItemGen->setNoPlate();
  PItemGen->generatePort(Control,keyName+"PipePort0",
			Geometry::Vec3D(0,0,0),Z);
  PItemGen->generatePort(Control,keyName+"PipePort1",
			 Geometry::Vec3D(0,0,0),-Z);
  PItemGen->setCF<CF40>(0.5*height+5.0);
  PItemGen->setNoPlate();
  PItemGen->generatePort(Control,keyName+"PipePort2",
			Geometry::Vec3D(0,0,0.5*shutterDistance),Y);
  PItemGen->generatePort(Control,keyName+"PipePort3",
			Geometry::Vec3D(0,0,-0.5*shutterDistance),Y);

  Control.addVariable(keyName+"ApertureBackLength",apertureBackLength);
  Control.addVariable(keyName+"ApertureInnerRadius",apertureInnerRadius);
  Control.addVariable(keyName+"ApertureMat","Tungsten");
  Control.addVariable(keyName+"ApertureOuterRadius",apertureOuterRadius);
  Control.addVariable(keyName+"ApertureThick",apertureThick);
  Control.addVariable(keyName+"ApertureToBlockGap",apertureToBlockGap);

  Control.addVariable(keyName+"BlockHeight",blockHeight);
  Control.addVariable(keyName+"BlockLength",blockLength);
  Control.addVariable(keyName+"BlockWidth",blockWidth);
  Control.addVariable(keyName+"ShutterDistance",shutterDistance);

  return;
}

  
}  // NAMESPACE setVariable
