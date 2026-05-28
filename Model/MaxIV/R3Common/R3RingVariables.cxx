/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3Common/R3RingVariables.cxx
 *
 * Copyright (c) 2004-2026 by S. Ansell and U. Friman-Gayer
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
// [11] Preliminary MAX IV Overview Drawing, Ring Dimensions: /mxn/groups/rad/Kvalitetshandbok-MAXIV/30_Normaldrift/1_SAR/Construction drawings/Briefing documents/2012-04-20/8-10.pdf
// [12] Ratchet End Wall Detail for Sectors 2-18 : {CDDIR}/08 K_20-6 Sektioner/K_20-6_633.pdf
// [13] Ratchet End Wall Detail for Sector 1 : {CDDIR}/08 K_20-6 Sektioner/K_20-6_634.pdf
// [14] Ratchet End Wall Detail for Sector 19 : {CDDIR}/08 K_20-6 Sektioner/K_20-6_635.pdf
// [15] Ratchet End Wall Detail for Sector 20 : {CDDIR}/08 K_20-6 Sektioner/K_20-6_636.pdf

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
    Set control variables and constants for the R3 ring.

    The present implementation is a simplification of the true ring geometry in the
    sense that it is closer to an ideal icosagon and assumes that all sectors have the
    same wall dimensions. The simplification is a good approximation for most beamlines
    along the ring. The model is only a rough approximation for the sectors near the 
    injection (1, 2, 19, and 20) which may feature thicker walls and a different
    ratchet-wall geometry.

    For more details, see the docstrings of individual variables. They describe how the
    variables were inferred from construction drawings and what approximations were
    made.

    \param Control :: Function data base to add constants to
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
  const double icosagonSideLengthToIncircleRadius = 1.0/(2.0*tan(M_PI/20.0));
  const double icosagonRadius = (
    icosagonWallLength*icosagonSideLengthToIncircleRadius
    +icosagonInnerWallThick+icosagonWallGapThick
  );
  Control.addVariable(preName+"IcosagonRadius",icosagonRadius);
  // Icosagon Wall Thickness from [5] (using the value for all N except 1, 2, and 20).
  const double icosagonWallThick = 100.0-icosagonWallGapThick;
  Control.addVariable(preName+"IcosagonWallThick",icosagonWallThick);
  // Outer Wall dimensions.
  // The Outer Wall is based on a icosagonal shape like the inner Icosagon Wall.
  // However, it looks more like a circular-saw blade due to the presence of the
  // ratchet end walls in each sector that provide an interface for the beamlines.
  //
  // Detailed drawings for the vicinity of the ratchet end walls in each sector are
  // available [5]. They even include the icosagon wall, but the author could neither
  // find a numerical value of the distance between the Icosagon Wall and the Outer
  // Wall, nor the length of the Outer-Wall sections. Therefore, these values have
  // been obtained and validated in a self consistent procedure which is described
  // below:
  //
  // For N = 1 and 2, the distance between two ratchet end walls on the inside of the
  // Outer Wall can be obtained by summing all dimensions in the top drawing in [4].
  // For all other sectors, no such information could be found. In the most general
  // case, all sectors would have individual lengths. However, by reconstructing the
  // Outer Wall in an external program, it could be demonstrated that a closed wall
  // can be built when all lengths are assumed to have the same value as N = 1 and 2.
  // Using this inner-wall length of 26679 mm from Ref. [4], a side length for the
  // inscribed icosagon of the Outer Wall can be determined. For 13 out of 20 sectors,
  // a value of 27059 mm is found. The standard deviation of all sectors is 1.4 mm with
  // a maximum deviation from the mean value of 5 mm. Even the largest deviation is on
  // the order of the rounding errors in the drawings. In the following, the majority
  // side length of 27059 mm will be used.
  //
  // From the inscribed icosagon side length of 27059 mm, one obtains an incircle
  // radius of 85422 mm. With the dimensions of the Icosagon Wall above
  // (IcosagonRadius = 78966 mm plus 750 mm wall thickness), the width of the
  // storage-ring tunnel (outside of Icosagon Wall to inside of Outer Wall) is found
  // to be 5706 mm. In terms of this class, this should be the value of OffsetCornerY.
  // Previously, a value of 5560 mm was used.  To the knowledge of the author,
  // OffsetCornerY is not given explicitly in the available drawings (except for a
  // value 5600 mm in Ref. [11], which is marked as preliminary).
  // However, it can be read off from Refs. [9] and [10] which show cross sections of
  // the storage-ring tunnel.
  // The resulting values of 5584(13) mm [9] and 5595 (18) mm [10] are in agreement
  // with each other, indicating that the tunnel width is the same in all sectors
  // (i.e additional wall thickness is added on the inside for the Icosagon Wall and on
  // the outside for the Outer Wall). It also agrees with the previously used value.
  // The origin of the ~140-mm discrepancy with the derived value of 5706 mm might be
  // attributed to the assumption that the Outer Wall is a regular icosagon.
  // The derived value will be used below for OffsetCornerY since it is based on
  // explicitly given dimensions.
  const double outerWallRadius = 2705.9*icosagonSideLengthToIncircleRadius;
  const double offsetCornerY = outerWallRadius-(icosagonRadius+icosagonWallThick);
  Control.addVariable(preName+"OffsetCornerY",offsetCornerY);
  // For the Outer Wall and Ratchet Wall dimensions, values that fit most of the
  // sectors (N = 2-18) [12] are chosen here. These dimensions are also used for
  // sectors 1, 19, and 20 although their wall design differs (see [13], [14],
  // and [15], respectively).
  // Due to the way the ring is constructed here, this choice also has an impact
  // on the beam radius.
  const double outerWallThick = 110.0; // [12]
  Control.addVariable(preName+"OuterWall",outerWallThick);
  Control.addVariable(preName+"OuterWallCut",-30.0);
  const double ratchetWallThickness = 160.0; // [12]
  Control.addVariable(preName+"RatchetWall",ratchetWallThickness);
  // The variable OffsetCornerX is the other parameter that determines the relative
  // positioning of the Outer Wall with respect to the Icosagon Wall.
  // Previously, the value was set to 716.0 without any reference.
  // In the following, OffsetCornerX is calculated from known quantities to reproduce
  // the correct dimensions of the Ratchet End Wall.
  // The drawing below illustrates the geometry. OffsetCornerX is inferred from the
  // condition that the line between P1 and P2 have an angle of 18 deg w.r.t. the
  // baseline.
  //
  //                           P2
  //                            *
  //                             \
  //                              \ Ratchet Wall Inner Length
  //             P1                \
  //              *                 |
  //               \                |
  //  OffsetCornerY \               | OffsetCornerY
  //                 \______________|
  //                   OffsetCornerX
  //
  // The ratchet wall inner length is also calculated from other known quantities.
  // The drawing below illustrates the geometry. The asterisk notes the intersection
  // between the ratchet end wall upstream side and the outer wall inside which 
  // determines the ratchet wall inner length.
  //
  //                     Ratchet Wall Thickness
  //                    ______________
  //       Outer Wall Thickness |     |
  //                    ________|     |  
  //                            |     |
  //  Ratchet Wall Inner Length |     |____  Ratchet Wall Step Length
  //                            |          \
  //                            *           \
  //                             \           \
  //                              \
  //                               \    Outer Wall Thickness
  const double icosagonAngle = M_PI*0.1;
  const double sin18 = sin(icosagonAngle);
  const double cos18 = cos(icosagonAngle);
  const double tan18 = tan(icosagonAngle);
  // Distance from inside of Outer Wall to Ratchet Wall step.
  // 1500 mm + 600 mm [12]
  const double ratchetWallStepToOuterWall = 210.0;
  // For the given sectors, Ref. [5] shows a value of 1036 mm for this quantity for the
  // majority of sectors (15). Two sectors have 1034 mm, and one sector has 1038 mm.
  // The discrepancies are possibly rounding issues and negligibly small. Therefore,
  // 1036 mm is used here.
  const double ratchetWallStepLength = 103.6;
  // See comment above on the derivation of this auxiliary quantity which is not given
  // in the drawings to the knowledge of the author.
  const double ratchetWallInnerLength = -(
    tan18*(ratchetWallThickness+ratchetWallStepLength-outerWallThick*sin18)
    -ratchetWallStepToOuterWall-outerWallThick*cos18
  );
  // See comment above on the derivation of OffsetCornerX.
  const double offsetCornerX = (
    offsetCornerY
    +cos(icosagonAngle)*(ratchetWallInnerLength-offsetCornerY)
    +tan18*sin(icosagonAngle)*(
      ratchetWallInnerLength-offsetCornerY
    )
  )/tan18;
  Control.addVariable(preName+"OffsetCornerX",offsetCornerX);
  // Adjust the beam radius such that it exits the Ratchet Wall at the correct position
  // for sectors N = 2-18 (see discussion above).
  //
  // Previously, the value of BeamRadius was set in the expression
  //
  // Control.addVariable(preName+"BeamRadius",8409.0-48.0); // 528m circum.
  //
  // No reference was given for these values. They result in a beam radius of
  // 8361 cm which would correspond to a circumference of 525 m. Since a radius
  // of 8409 cm results in the correct circumference as given in the original comment,
  // it is assumed that the 48 cm were subtracted later to fix a discrepancy in the
  // model.
  // The current BeamRadius has a value of 8392 cm (527 m circumference).
  Control.addVariable(preName+"BeamRadius",outerWallRadius-150.0); // [12]

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
