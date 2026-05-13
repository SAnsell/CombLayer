/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3Common/R3RingVariables.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "RingDoorGenerator.h"

#include "PipeGenerator.h"
#include "CornerPipeGenerator.h"
#include "PipeTubeGenerator.h"
#include "VacuumBoxGenerator.h"
#include "BellowGenerator.h"
#include "CrossGenerator.h"
#include "PortItemGenerator.h"
#include "HeatDumpGenerator.h"
#include "GateValveGenerator.h"
#include "CylGateValveGenerator.h"
#include "BeamMountGenerator.h"
#include "WallLeadGenerator.h"

#include "EPSeparatorGenerator.h"
#include "R3ChokeChamberGenerator.h"
#include "MagnetM1Generator.h"
#include "MagnetU1Generator.h"
#include "HeatAbsorberToyamaGenerator.h"
#include "BremBlockGenerator.h"
#include "ProximityShieldingGenerator.h"

#include "maxivVariables.h"

// References [4-15] refer to construction drawings of MAX IV by the company Wihlborgs.
// The given paths are relative to the top-level directory:
// CDDIR = /mxn/groups/rad/Kvalitetshandbok-MAXIV/30_Normaldrift/1_SAR/Construction drawings/Wihlborgs complete
// Many drawings follow a systematic naming scheme that includes the beamline number
// N (1 <= N <= 20). In the file paths, N should be inserted as a zero-padded, 
// two-digit integer, i.e. for beamline 3, Refs. [14] and [15] are the files
//
// .../K_15-1_A10-031.pdf
//      and
// .../K_20-2_603.pdf
//
// respectively.
// 
// References
// [1] ForMAX and MicroMAX Frontend Technical Specification
//     http://localhost:8080/maxiv/work-log/micromax/pictures/front-end/formax-and-micromax-frontend-technical-specification.pdf/view
// [2] Toyama ForMAX Mechanical drawings
//     http://localhost:8080/maxiv/work-log/tomowise/toyama_formax_fe_mechanical_drawings.pdf/view
// [3] TomoWISE Mask Evaluation (email from PI 20 Feb 2025)
//
// [4] Outer Wall Inner Side for Sectors 1,2: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_647.pdf
// [5] Outer Wall Inner Side for Sectors 3,4: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_648.pdf
// [6] Outer Wall Inner Side for Sectors 5,6: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_649.pdf
// [7] Outer Wall Inner Side for Sectors 7,8: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_650.pdf
// [8] Outer Wall Inner Side for Sectors 9,10: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_651.pdf
// [9] Outer Wall Inner Side for Sectors 11,12: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_652.pdf
// [10] Outer Wall Inner Side for Sectors 13,14: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_653.pdf
// [11] Outer Wall Inner Side for Sectors 15,16: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_654.pdf
// [12] Outer Wall Inner Side for Sectors 17,18: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_655.pdf
// [13] Outer Wall Inner Side for Sectors 19,20: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_656.pdf
// [14] Sector Top View Showing Icosagon Wall and Outer Wall: {CDDIR}/04 K_15-1 Planer - Grund- och golv - Mått/K_15-1_A10-{NN}1.pdf
// [15] Icosagon Wall Inner Side: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_6{NN}.pdf

namespace setVariable
{

void collimatorVariables(FuncDataBase&,const std::string&);
void wallVariables(FuncDataBase&,const std::string&);
void R3RingDoors(FuncDataBase& Control,const std::string& preName);
void R3RingVariables(FuncDataBase& Control);

void
collimatorVariables(FuncDataBase& Control,const std::string& collKey)
  /*!
    Builds the variables for the collimator
    \param Control :: Database
    \param collKey :: prename
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","collimatorVariables");

  Control.addVariable(collKey+"Width",4.0);
  Control.addVariable(collKey+"Height",4.0);
  Control.addVariable(collKey+"Length",16.0);
  Control.addVariable(collKey+"InnerAWidth",1.2);
  Control.addVariable(collKey+"InnerAHeight",1.2);
  Control.addVariable(collKey+"InnerBWidth",1.2);
  Control.addVariable(collKey+"InnerBHeight",1.2);
  Control.addVariable(collKey+"Mat","Tantalum");

  return;
}

void
wallVariables(FuncDataBase& Control,const std::string& wallKey)
 /*!
    Set the variables for the frontend wall
    \param Control :: DataBase to use
    \param wallKey :: name before part names
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","wallVariables");

  WallLeadGenerator LGen;
  LGen.setWidth(150.0);
  LGen.generateWall(Control,wallKey,3.0);

  return;
}

void
R3RingDoors(FuncDataBase& Control,const std::string& preName)
  /*!
    Construct variables for R3RingDoors
    \param Control :: Control value
    \param preName :: Prename (r3ring typically)
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3RingDoors");

  RingDoorGenerator RGen;

  Control.addVariable(preName+"RingDoorWallID",1);

  RGen.generateDoor(Control,preName+"RingDoor",2400.0);
  return;
}

void
R3RingVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    for the R3 concrete shielding walls
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("R3RingVariables[F]","R3RingVariables");

  const std::string preName("R3Ring");

  Control.addVariable(preName+"FullOuterRadius",14000.0); // Arbitrary
  Control.addVariable(preName+"IcosagonRadius",7865.0);       // U
  Control.addVariable(preName+"BeamRadius",8409.0-48.0);       // 528m circum.
  Control.addVariable(preName+"IcosagonWallThick",90.0);
  Control.addVariable(preName+"OffsetCornerX",716.0);
  Control.addVariable(preName+"OffsetCornerY",556.0);
  Control.addVariable(preName+"OuterWall",110.0);
  Control.addVariable(preName+"OuterWallCut",-40.0);
  // In reality, REW have different thickness, but we have the same for all.
  Control.addVariable(preName+"RatchetWall",160.0); // for TomoWISE (standard thickness) K_20-6_633
  ELog::EM << "Bulk shielding thick depends on the beamline. Currently, the same for all" << ELog::endWarn;

  Control.addVariable(preName+"Insulation",10.0);
  Control.addVariable(preName+"InsulationCut",400.0);
  Control.addVariable(preName+"InsulationDepth",80.0);


  Control.addVariable(preName+"Height",180.0);
  Control.addVariable(preName+"Depth",130.0);
  Control.addVariable(preName+"RoofThick",100.0);
  Control.addVariable(preName+"FloorThick",100.0);

  Control.addVariable(preName+"WallMat","Concrete");
  Control.addVariable(preName+"FloorMat","Concrete");
  Control.addVariable(preName+"RoofMat","Concrete");

  R3RingDoors(Control,preName);

  return;
}

}  // NAMESPACE setVariable
