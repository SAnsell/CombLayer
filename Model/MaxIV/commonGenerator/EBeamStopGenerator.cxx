/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/EBeamStopGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "EBeamStopGenerator.h"

namespace setVariable
{

EBeamStopGenerator::EBeamStopGenerator() :
  width(11.4),length(41.4),height(11.2),depth(10.0),
  wallThick(0.8),baseThick(2.6),baseFlangeExtra(3.0),
  baseFlangeThick(2.6),portLength(4.4),
  portRadius(CF40_22::innerRadius),
  portWallThick(CF40_22::innerRadius),
  portFlangeRadius(CF40_22::flangeRadius),
  portFlangeLength(CF40_22::flangeLength),
  stopRadius(6.0),stopLength(35.7),stopZLift(6.1),    
  stopLen({32.0}),
  supportConeLen(5.6),supportConeRadius(2.7),
  supportHoleRadius(1.0),supportRadius(2.0),
  stopPortYStep(5.7),stopPortRadius(CF63::innerRadius),
  stopPortLength(31.7),stopPortWallThick(CF63::wallThick),
  stopPortFlangeRadius(CF63::flangeRadius),
  stopPortFlangeLength(CF63::flangeLength),
  stopPortPlateThick(CF63::flangeLength),
  ionPortYStep(-13.4),ionPortRadius(CF63::innerRadius),
  ionPortLength(30.0),ionPortWallThick(CF63::wallThick),
  ionPortFlangeRadius(CF63::flangeRadius),
  ionPortFlangeLength(CF63::flangeLength),
  ionPortPlateThick(CF63::flangeLength),

  stopMat({"Stainless304L","Lead"}),
  voidMat("Void"),wallMat("Stainless304L"),
  flangeMat("Stainless304L"),
  plateMat("Stainless304L"),
  outerMat("Void")
  /*!
    Constructor and defaults
  */
{}

EBeamStopGenerator::~EBeamStopGenerator()
 /*!
   Destructor
 */
{}


void
EBeamStopGenerator::generateEBeamStop(FuncDataBase& Control,
				      const std::string& keyName,
				      const bool closedFlag)  const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("EBeamStopGenerator","generateEBeamStop");

  Control.addVariable(keyName+"Closed",static_cast<size_t>(closedFlag));
  
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"BaseFlangeExtra",baseFlangeExtra);
  Control.addVariable(keyName+"BaseFlangeThick",baseFlangeThick);
  Control.addVariable(keyName+"PortLength",portLength);
  Control.addVariable(keyName+"PortRadius",portRadius);
  Control.addVariable(keyName+"PortWallThick",portWallThick);
  Control.addVariable(keyName+"PortFlangeRadius",portFlangeRadius);
  Control.addVariable(keyName+"PortFlangeLength",portFlangeLength);
  Control.addVariable(keyName+"StopRadius",stopRadius);
  Control.addVariable(keyName+"StopLength",stopLength);
  Control.addVariable(keyName+"StopZLift",stopZLift);

  Control.addVariable(keyName+"SupportConeLen",supportConeLen);
  Control.addVariable(keyName+"SupportConeRadius",supportConeRadius);
  Control.addVariable(keyName+"SupportHoleRadius",supportHoleRadius);
  Control.addVariable(keyName+"SupportRadius",supportRadius);

  Control.addVariable(keyName+"StopPortYStep",stopPortYStep);
  Control.addVariable(keyName+"StopPortRadius",stopPortRadius);
  Control.addVariable(keyName+"StopPortLength",stopPortLength);
  Control.addVariable(keyName+"StopPortWallThick",stopPortWallThick);
  Control.addVariable(keyName+"StopPortDepth",stopPortDepth);
  Control.addVariable(keyName+"StopPortFlangeRadius",stopPortFlangeRadius);
  Control.addVariable(keyName+"StopPortFlangeLength",stopPortFlangeLength);
  Control.addVariable(keyName+"StopPortPlateThick",stopPortPlateThick);

 Control.addVariable(keyName+"IonPortYStep",ionPortYStep);
 Control.addVariable(keyName+"IonPortRadius",ionPortRadius);
 Control.addVariable(keyName+"IonPortLength",ionPortLength);
 Control.addVariable(keyName+"IonPortWallThick",ionPortWallThick);
 Control.addVariable(keyName+"IonPortFlangeRadius",ionPortFlangeRadius);
 Control.addVariable(keyName+"IonPortFlangeLength",ionPortFlangeLength);
 Control.addVariable(keyName+"IonPortPlateThick",ionPortPlateThick);

 Control.addVariable(keyName+"VoidMat",voidMat);
 Control.addVariable(keyName+"WallMat",wallMat);
 Control.addVariable(keyName+"FlangeMat",flangeMat);
 Control.addVariable(keyName+"PlateMat",plateMat);
 Control.addVariable(keyName+"OuterMat",outerMat);

 const size_t NS(stopLen.size());
 Control.addVariable(keyName+"StopNLen",NS);
 for(size_t i=0;i<NS;i++)
   {
     const std::string nStr=std::to_string(i);
     Control.addVariable(keyName+"StopLen"+nStr,stopLen[i]);
     Control.addVariable(keyName+"StopMat"+nStr,stopMat[i]);
   }
 Control.addVariable(keyName+"StopMat"+std::to_string(NS),stopMat[NS]);
 
 return;

}


}  // NAMESPACE setVariable
