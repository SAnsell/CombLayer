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
#include "MonoShutterR3Generator.h"

namespace setVariable
{
template<typename MainFlange,typename EntryExitFlange,
typename ShutterFlange,typename AdapterFlange>
MonoShutterR3Generator<MainFlange,EntryExitFlange,ShutterFlange,AdapterFlange>
::MonoShutterR3Generator() :
  height(29.8), // [5]
  length(30.5), // [1]
  width(22.0),
  adapterInnerRadius(AdapterFlange::innerRadius),
  beamPortInnerRadius(EntryExitFlange::innerRadius),
  beamPortWallThick(EntryExitFlange::wallThick),
  beamPortFlangeRadius(EntryExitFlange::flangeRadius),
  beamPortFlangeLength(EntryExitFlange::flangeLength),
  vesselInnerRadius(MainFlange::innerRadius),
  vesselWallThick(MainFlange::wallThick),
  vesselFlangeRadius(MainFlange::flangeRadius),
  vesselFlangeLength(4.8), // [5]
  vesselMat("SteelUnknownGrade"), // [1]
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
  blockMat("Tungsten"), // [2] see comment on tungsten material above
  shutterDistance(7.05), // [5]
  shutterPortLength(19.0), // [5]
  shutterPortInnerRadius(ShutterFlange::innerRadius), // [5]
  shutterPortWallThick(ShutterFlange::wallThick), // [5]
  shutterPortFlangeRadius(ShutterFlange::flangeRadius), // [5]
  shutterPortFlangeLength(ShutterFlange::flangeLength), // [5]
  threadLength(28.0), // [5]
  threadRadius(1.5), // [5]
  threadMat("SteelUnknownGrade"), // [1]
  lift(5.0), // [5]
  entryShutterUpFlag(false),
  exitShutterUpFlag(false)
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
::generate(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("MonoShutterR3Generator","generate");

  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);

  Control.addVariable(keyName+"AdapterInnerRadius",adapterInnerRadius);

  Control.addVariable(keyName+"BeamPortInnerRadius",beamPortInnerRadius);
  Control.addVariable(keyName+"BeamPortWallThick",beamPortWallThick);
  Control.addVariable(keyName+"BeamPortFlangeRadius",beamPortFlangeRadius);
  Control.addVariable(keyName+"BeamPortFlangeLength",beamPortFlangeLength);

  Control.addVariable(keyName+"VesselInnerRadius",vesselInnerRadius);
  Control.addVariable(keyName+"VesselWallThick",vesselWallThick);
  Control.addVariable(keyName+"VesselFlangeRadius",vesselFlangeRadius);
  Control.addVariable(keyName+"VesselFlangeLength",vesselFlangeLength);
  Control.addVariable(keyName+"VesselMat",vesselMat);

  Control.addVariable(keyName+"ApertureBackLength",apertureBackLength);
  Control.addVariable(keyName+"ApertureInnerRadius",apertureInnerRadius);
  Control.addVariable(keyName+"ApertureMat",apertureMat);
  Control.addVariable(keyName+"ApertureOuterRadius",apertureOuterRadius);
  Control.addVariable(keyName+"ApertureThick",apertureThick);
  Control.addVariable(keyName+"ApertureToBlockGap",apertureToBlockGap);

  Control.addVariable(keyName+"BlockHeight",blockHeight);
  Control.addVariable(keyName+"BlockLength",blockLength);
  Control.addVariable(keyName+"BlockWidth",blockWidth);
  Control.addVariable(keyName+"BlockMat",blockMat);

  Control.addVariable(keyName+"ShutterDistance",shutterDistance);
  Control.addVariable(keyName+"ShutterPortLength",shutterPortLength);
  Control.addVariable(keyName+"ShutterPortInnerRadius",shutterPortInnerRadius);
  Control.addVariable(keyName+"ShutterPortWallThick",shutterPortWallThick);
  Control.addVariable(keyName+"ShutterPortFlangeRadius",shutterPortFlangeRadius);
  Control.addVariable(keyName+"ShutterPortFlangeLength",shutterPortFlangeLength);
  Control.addVariable(keyName+"ThreadLength",threadLength);
  Control.addVariable(keyName+"ThreadRadius",threadRadius);
  Control.addVariable(keyName+"ThreadMat",threadMat);
  Control.addVariable(keyName+"Lift",lift);

  Control.addVariable(keyName+"EntryShutterUpFlag",
    static_cast<int>(entryShutterUpFlag));
  Control.addVariable(keyName+"ExitShutterUpFlag",
    static_cast<int>(exitShutterUpFlag));

  return;
}

template MonoShutterR3Generator<CF200,CF63,CF40,CF40>::MonoShutterR3Generator();
template MonoShutterR3Generator<CF200,CF63,CF40,CF40>::~MonoShutterR3Generator();
template void MonoShutterR3Generator<CF200,CF63,CF40,CF40>::generate(
  FuncDataBase& Control,const std::string& keyName) const;

}  // NAMESPACE setVariable
