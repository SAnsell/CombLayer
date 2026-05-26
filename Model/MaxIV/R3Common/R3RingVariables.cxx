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
// CDDIR = /mxn/groups/rad/Kvalitetshandbok-MAXIV/30_Normaldrift/1_SAR/Construction drawings/Wihlborgs complete/K
// Many drawings follow a systematic naming scheme that includes the beamline/sector
// number N (1 <= N <= 20).
//
// In Refs. [5] and [6], where one file exists for each sector, N should be
// inserted as a zero-padded, two-digit integer, i.e. for sector 3, Refs. [14] and
// [15] are the files
//
// .../K_15-1_A10-031.pdf
//      and
// .../K_20-2_603.pdf
//
// respectively.
//
// In Ref. [4], where one file describes the geometry of a pair of sectors, a
// mathematical expression is given to calculate the file index. For example, for
// sector 4, Ref. [4] is the file
//
// .../K_20-2_648.pdf
//
// because
//
// 647+⌊(4-1)/2⌋ = 647+⌊1.5⌋ = 647+1 = 648
//
// Refs. [7] and [8] contain the dimensions of the icosagon wall doors. Ref. [7] is
// referred to by Refs. [5] for N=1,2, and 20 (sectors with the thicker icosagon wall),
// while Ref. [8] is referred to by all the remaining sectors except N=17. It is
// assumed that the label has simply been forgotten there and that Ref. [8] also 
// applies to sector N=17 for reasons of symmetry.
// The situation is analogous for Refs. [9] and [10] where the former applies to
// sectors N=1,2, and 20, and the latter to all others (special case N=17).
//
// References
// [1] ForMAX and MicroMAX Frontend Technical Specification
//     http://localhost:8080/maxiv/work-log/micromax/pictures/front-end/formax-and-micromax-frontend-technical-specification.pdf/view
// [2] Toyama ForMAX Mechanical drawings
//     http://localhost:8080/maxiv/work-log/tomowise/toyama_formax_fe_mechanical_drawings.pdf/view
// [3] TomoWISE Mask Evaluation (email from PI 20 Feb 2025)
//
// [4] Outer Wall Inner Side: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_{647+⌊(N-1)/2⌋}.pdf
// [5] Sector Top View Showing Icosagon Wall and Outer Wall: {CDDIR}/04 K_15-1 Planer - Grund- och golv - Mått/K_15-1_A10-{NN}1.pdf
// [6] Icosagon Wall Inner Side: {CDDIR}/07 K_20-2 Väggelevationer - Mått/K_20-2_6{NN}.pdf
// [7] Icosagon Wall Door for Sectors 1,2,20: {CDDIR}/08 K_20-6 Sektioner/K_20-6_611.pdf
// [8] Icosagon Wall Door for Sectors 3-16, and 18-20: {CDDIR}/08 K_20-6 Sektioner/K_20-6_621.pdf
// [9] Storage Ring Tunnel Cross Section for Sectors 1,2,20, : {CDDIR}/08 K_20-6 Sektioner/K_20-6_610.pdf
// [10] Storage Ring Tunnel Cross Section for Sectors 3-16, and 18-20: {CDDIR}/08 K_20-6 Sektioner/K_20-6_620.pdf

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
  // Radius of the incircle of the Icosagon Wall.
  // At positions where the icosagon wall consists of parallel walls only, the
  // following layers can be found in all sectors [5]
  // (going radially from inside to outside):
  //
  // * 300 mm concrete
  // * 100 mm gap (dimensions not shown in [5], but in [7] and [8])
  // * 150 mm gap
  // * 100 mm gap
  // * 1250 mm (N=1,2, and 20) or 750 mm (all others) concrete
  //
  // (For N=1,2, and 20, it should be noted that there is an error in the drawings
  // where the last three layers are falsely given as "150+100+750". By comparison to
  // the nearby door dimensions and the drawings for all other sectors, it is clear
  // that this should be "150+100+1250").
  //
  // In reference [6], the total wall length at the inside of the icosagon wall can be
  // read off by summing the six or seven dimensions given at the top of the A-A cut.
  // For all sectors except N=2, 3, 19, and 20 (i.e the sectors where a change of the
  // icosagon-wall thickness occurs), this corresponds to the length between the two
  // sector planes, as can be verified by comparing to Ref. [5].
  // For all sectors except N=1, 2, 3, 19, and 20, the inner wall has the same length
  // of 24808 mm. For N=1 (24644 mm), the difference is on the order of 20 centimeters,
  // for N=2, 3, 19, and 20, it is on the order of 2 meters.
  // In the present implementation, which needs a regular icosagon, the majority wall
  // length of 24808 mm is used to obtain the incircle radius.
  const double icosagonWallLength = 2480.8;
  // The present implementation does not include the innermost layer
  // (300 mm concrete + 100 mm gap)
  const double icosagonInnerWallThick = 40.0;
  // Of the given 1250 mm (N=1,2, and 20) or 750 mm (all others), the innermost 250 mm
  // are part of a gap.
  const double icosagonWallGapThick = 25.0;
  Control.addVariable(preName+"IcosagonRadius",
    icosagonWallLength/(2.0*tan(M_PI/20.0))
    +icosagonInnerWallThick+icosagonWallGapThick
  );
  Control.addVariable(preName+"BeamRadius",8409.0-48.0);       // 528m circum.
  // Icosagon Wall Thickness from [5] (using the value for all N except 1, 2, and 20).
  Control.addVariable(preName+"IcosagonWallThick",100.0-icosagonWallGapThick);
  Control.addVariable(preName+"OffsetCornerX",716.0);
  Control.addVariable(preName+"OffsetCornerY",556.0);
  Control.addVariable(preName+"OuterWall",110.0);
  // Adjusted for (roughly) 600 mm distance to optical axis for a DanMAX-type beamline.
  Control.addVariable(preName+"OuterWallCut",-30.0);
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
