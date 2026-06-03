/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/MLMono.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell and U. Friman-Gayer
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
#ifndef xraySystem_MLMono_h
#define xraySystem_MLMono_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMono
  \author S. Ansell and U. Friman-Gayer
  \version 1.1
  \date June 2026
  \brief Multilayer monochromator (MLM) with two structurally equivalent crystals

  This is a simplified on an FMB Oxford MLM for DanMAX [1,2].
  It consists of two disjoint crystal elements ("A" and "B") that are structurally
  equivalent. Rotations and most dimensions of the two elements can be set
  individually.
  The typically very thin MLM crystal layers are not modeled.
  
  References:
  [1] S3716 DanMAX MLM Functional Specification Rev05_NEW.pdf
  [2] /mxn/groups/rad/Beamlines/DanMAX/Optics/FDR_DCM_MLM/Stp Files/MLM/ADM0342 - MLM Top Level.stp

  Version History:
  1.1 - 2026-06-03
    - Add disaster masks
  1.0 - 2019
*/

class MLMono :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double gap;               ///< Gap thickness

  double thetaA;             ///< Theta angle [mirror -xray 2T]
  double thetaB;             ///< Theta angle [mirror -xray 2T]
  double phiA;             ///< Theta angle [mirror -xray 2T]
  double phiB;             ///< Theta angle [mirror -xray 2T]

  double widthA;            ///< Width of block across beam
  double heightA;           ///< Depth into beam
  double lengthA;           ///< Length along beam

  double widthB;            ///< Width of block across beam
  double heightB;           ///< Depth into beam
  double lengthB;           ///< Length along beam

  double supportAGap;         ///< Gap after mirror (before back)
  double supportAExtra;       ///< Base/Top extra length
  double supportABackThick;   ///< Back thickness
  double supportABackLength;  ///< Back lenght (in part)
  double supportABase;        ///< Base/Top thickness
  double supportAPillar;      ///< Side unit radius
  double supportAPillarStep;  ///< Side unit step

  double supportBGap;         ///< Gap after mirror (before back)
  double supportBExtra;       ///< Base/Top extra length
  double supportBBackThick;   ///< Back thickness
  double supportBBackLength;  ///< Back lenght (in part)
  double supportBBase;        ///< Base/Top thickness
  double supportBPillar;      ///< Side unit radius
  double supportBPillarStep;  ///< Side unit step

  int parked;                   ///< Flag to set crystals in parked position
  double parkedOffset;         ///< y-Offset between front surfaces of crystals in parked position
  double parkedGap;             ///< x-Gap between crystals in parked position

  double disasterMaskAWidth;   ///< Mask width
  double disasterMaskALength;  ///< Mask length
  double disasterMaskACornerSideLength;  ///< Mask corner side length
  double disasterMaskAYStep;   ///< Mask y step (if 0, aligned with support front)
  double disasterMaskBWidth;   ///< Mask width
  double disasterMaskBLength;  ///< Mask length
  double disasterMaskBCornerSideLength;  ///< Mask corner side length
  double disasterMaskBYStep;   ///< Mask y step (if 0, aligned with support front)

  int mirrorAMat;             ///< XStal material
  int mirrorBMat;             ///< XStal material
  int baseAMat;               ///< Base material
  int baseBMat;               ///< Base material
  int disasterMaskAMat;       ///< Disaster Mask material
  int disasterMaskBMat;       ///< Disaster Mask material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMono(const std::string&);
  MLMono(const MLMono&);
  MLMono& operator=(const MLMono&);
  ~MLMono() override;

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
