/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/MLMonoGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef setVariable_MLMonoGenerator_h
#define setVariable_MLMonoGenerator_h

namespace setVariable
{

/*!
  \class MLMonoGenerator
  \author S. Ansell, K. Batkov, and U. Friman-Gayer
  \version 1.1
  \date June 2026
  \brief Variable generator for MLMono

  Using dimensions of an FMB Oxford MLM for DanMAX as default.

  References:
  [1] Email from MJ 260506
  [2] /mxn/groups/rad/Beamlines/DanMAX/Optics/FDR_DCM_MLM/Stp Files/MLM/ADM0342 - MLM Top Level.stp
  [3] S3716 DanMAX MLM Functional Specification Rev05_NEW.pdf

  Version History:
  1.1 - 2026-06-03
    - Add references and update default parameters.
    - Update for MLMono v1.1
  1.0 - 2019
*/

class MLMonoGenerator
{
 private:

  double gap;                  ///< Gap thickness
  double phiA;                 ///< Mono Pitch
  double phiB;                 ///< Mono Pitch

  double widthA;               ///< Width of block across beam
  double heightA;              ///< Depth into beam
  double lengthA;              ///< Length along beam

  double widthB;               ///< Width of block across beam
  double heightB;              ///< Depth into beam
  double lengthB;              ///< Length along beam

  double supportAGap;          ///< Gap after mirror (before back)
  double supportAExtra;        ///< Base/Top extra length
  double supportABackThick;    ///< Back thickness
  double supportABackLength;   ///< Back lenght (in part)
  double supportABase;         ///< Base/Top thickness
  double supportAPillar;       ///< Side unit radius
  double supportAPillarStep;   ///< Side unit step

  double supportBGap;          ///< Gap after mirror (before back)
  double supportBExtra;        ///< Base/Top extra length
  double supportBBackThick;    ///< Back thickness
  double supportBBackLength;   ///< Back lenght (in part)
  double supportBBase;         ///< Base/Top thickness
  double supportBPillar;       ///< Side unit radius
  double supportBPillarStep;   ///< Side unit step

  int parked;                   ///< Flag to set crystals in parked position
  double parkedOffset;         ///< Offset between front surfaces of crystals in parked position
  double parkedGap;             ///< x-Gap between crystals in parked position

  double disasterMaskACornerSideLength;  ///< Mask corner side length
  double disasterMaskALength;  ///< Mask length
  double disasterMaskAWidth;   ///< Mask width
  double disasterMaskAYStep;   ///< Mask y step (if 0, aligned with support front)
  double disasterMaskBCornerSideLength;  ///< Mask corner side length
  double disasterMaskBLength;  ///< Mask length
  double disasterMaskBWidth;   ///< Mask width
  double disasterMaskBYStep;   ///< Mask y step (if 0, aligned with support front)

  std::string mirrorAMat;             ///< XStal material
  std::string mirrorBMat;             ///< XStal material
  std::string baseAMat;               ///< Base material
  std::string baseBMat;               ///< Base material
  std::string disasterMaskAMat;               ///< Disaster Mask material
  std::string disasterMaskBMat;               ///< Disaster Mask material

 public:

  MLMonoGenerator();
  MLMonoGenerator(const MLMonoGenerator&);
  MLMonoGenerator& operator=(const MLMonoGenerator&);
  virtual ~MLMonoGenerator();

  /// accessor to gap
  void setGap(const double G) { gap=G; }
  void generateMono(FuncDataBase&,const std::string&,
		    const double,const double,const double) const;

};

}

#endif
