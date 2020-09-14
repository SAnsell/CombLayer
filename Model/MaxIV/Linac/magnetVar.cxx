/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/magVariables.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell/Konstantin Batkov
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
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "SplitPipeGenerator.h"
#include "BellowGenerator.h"
#include "GateValveGenerator.h"
#include "CorrectorMagGenerator.h"
#include "LinacQuadGenerator.h"
#include "LinacSexuGenerator.h"
#include "PortTubeGenerator.h"
#include "JawFlangeGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "StriplineBPMGenerator.h"
#include "CylGateValveGenerator.h"
#include "GateValveGenerator.h"
#include "DipoleDIBMagGenerator.h"
#include "EArrivalMonGenerator.h"
#include "YagScreenGenerator.h"
#include "YagUnitGenerator.h"
#include "YagUnitBigGenerator.h"
#include "FlatPipeGenerator.h"
#include "TriPipeGenerator.h"
#include "TriGroupGenerator.h"
#include "BeamDividerGenerator.h"
#include "ScrapperGenerator.h"
#include "SixPortGenerator.h"
#include "CeramicGapGenerator.h"
#include "EBeamStopGenerator.h"
#include "TWCavityGenerator.h"
#include "UndVacGenerator.h"
#include "FMUndulatorGenerator.h"
#include "subPipeUnit.h"
#include "MultiPipeGenerator.h"
#include "ButtonBPMGenerator.h"
#include "CurveMagGenerator.h"
#include "CleaningMagnetGenerator.h"

#include "magnetVar.h"

namespace setVariable
{

namespace linacVar
{

void
Segment32Magnet(FuncDataBase& Control,
		   const std::string& lKey)
  /*!
    This should be for the magnet unit but currently doing segment32
    to make fast compiles
    \param Control :: Variable Database
    \param lKey :: key name
  */
{
  ELog::RegMethod RegA("linacVariables[F]","Segment32Magnet");

  setVariable::PipeGenerator PGen;
  setVariable::FlatPipeGenerator FPGen;
  setVariable::DipoleDIBMagGenerator DIBGen;
  setVariable::BellowGenerator BellowGen;

  setFlat(Control,lKey+"FlatA",82.581,0.0);

  DIBGen.generate(Control,lKey+"DMA");
  Control.addVariable(lKey+"DMAXStep",-0.5198);
  Control.addVariable(lKey+"DMAYStep",0.0009);

  PGen.setMat("Stainless316L","Stainless304L");
  PGen.setNoWindow();
  PGen.setCF<CF18_TDC>();

  PGen.generatePipe(Control,lKey+"PipeA",94.401); // No_32_34_00
  Control.addVariable(lKey+"PipeAXYAngle",-1.6); // No_32_34_00

  setFlat(Control,lKey+"FlatB",82.582,-1.6); // measured

  DIBGen.generate(Control,lKey+"DMB");
  Control.addVariable(lKey+"DMBXStep",-0.23505);
  Control.addVariable(lKey+"DMAYStep",0.02628);

  setBellow26(Control,lKey+"Bellow",7.501);
  Control.addVariable(lKey+"BellowXYAngle",-1.6);

  return;
}

}  // NAMESPACE linacVAR

}   // NAMESPACE setVariable