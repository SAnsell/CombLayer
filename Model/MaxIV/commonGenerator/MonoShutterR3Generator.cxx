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
template<typename MainFlange,typename EntryExitFlange,
typename ShutterFlange,typename AdapterFlange>
MonoShutterR3Generator<MainFlange,EntryExitFlange,ShutterFlange,AdapterFlange>
::MonoShutterR3Generator() :
  PTubeGen(new PipeTubeGenerator()),
  PItemGen(new PortItemGenerator()),
  SUnitGen(new ShutterUnitGenerator()),
  apertureBackLength(7.0), // [4]
  apertureInnerRadius(1.0), // [3,4]
  // TODO: Should be an alloy with > 95% tungsten, not pure tungsten.
  apertureMat("Tungsten"), // [3,4]
  apertureOuterRadius(5.15), // [3,4]
  apertureThick(1.0), // [3,4]
  apertureToBlockGap(0.4), // [1]
  blockHeight(5.0), // [2]
  blockLength(5.0), // [2]
  blockWidth(5.0), // [2]
  flangeThick(1.225), // [5]
  height(25.4), // [5]
  length(30.5), // [1]
  shutterDistance(7.05) // [5]
  /*!
    Constructor and defaults
  */
{}

template<typename MainFlange,typename EntryExitFlange,
typename ShutterFlange,typename AdapterFlange>
MonoShutterR3Generator<MainFlange,EntryExitFlange,ShutterFlange,AdapterFlange>
::~MonoShutterR3Generator() 
 /*!
   Destructor
 */
{}

template<typename MainFlange,typename EntryExitFlange,
typename ShutterFlange,typename AdapterFlange>
void MonoShutterR3Generator<MainFlange,EntryExitFlange,ShutterFlange,AdapterFlange>
::generateShutter(FuncDataBase& Control,
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

  SUnitGen->setCF<ShutterFlange>();
  // TODO: Should be an alloy with > 95% tungsten, not pure tungsten.
  SUnitGen->setBlock(blockHeight,blockLength,blockWidth,"Tungsten");
  SUnitGen->generateShutter(Control,keyName+"UnitA",upFlagA);
  SUnitGen->generateShutter(Control,keyName+"UnitB",upFlagB);
  
  PTubeGen->setCF<MainFlange>();
  PTubeGen->setAFlange(MainFlange::flangeRadius,flangeThick);
  PTubeGen->setBFlange(MainFlange::flangeRadius,flangeThick);
  PTubeGen->setCap(1,1);
  PTubeGen->generateTube(Control,keyName+"Pipe",
    height-2.0*flangeThick);
  Control.addVariable(keyName+"PipeNPorts",4);

  PTubeGen->setPipe(AdapterFlange::innerRadius,
    EntryExitFlange::flangeRadius-AdapterFlange::innerRadius,1.0,0.0);
  PTubeGen->generateTube(Control,keyName+"EntryAdapter",EntryExitFlange::flangeLength);
  Control.addVariable(keyName+"EntryAdapterNPorts",0);
  PTubeGen->generateTube(Control,keyName+"ExitAdapter",EntryExitFlange::flangeLength);
  Control.addVariable(keyName+"ExitAdapterNPorts",0);
  
  const Geometry::Vec3D Y(0,1,0);
  const Geometry::Vec3D Z(0,0,1);
  PItemGen->setCF<EntryExitFlange>(0.5*length-EntryExitFlange::flangeLength);
  PItemGen->setNoPlate();
  PItemGen->generatePort(Control,keyName+"PipePort0",
			Geometry::Vec3D(0,0,0),Z);
  PItemGen->generatePort(Control,keyName+"PipePort1",
			 Geometry::Vec3D(0,0,0),-Z);
  PItemGen->setCF<ShutterFlange>(0.5*height+5.0); // [5]
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

template MonoShutterR3Generator<CF200,CF63,CF40,CF40>::MonoShutterR3Generator();
template MonoShutterR3Generator<CF200,CF63,CF40,CF40>::~MonoShutterR3Generator();
template void MonoShutterR3Generator<CF200,CF63,CF40,CF40>::generateShutter(
  FuncDataBase& Control,const std::string& keyName,const bool upFlagA,
  const bool upFlagB) const;

}  // NAMESPACE setVariable
