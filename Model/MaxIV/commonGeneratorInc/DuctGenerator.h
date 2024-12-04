/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/DuctGenerator.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef setVariable_DuctGenerator_h
#define setVariable_DuctGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DuctGenerator
  \version 1.0
  \author Konstantin Batkov
  \date August 2023
  \brief DuctGenerator for variables
*/

class DuctGenerator
{
 private:

  double length;                ///< Duct length
  double width;                 ///< Width
  double height;                ///< Height
  double radius;                 ///< Radius (if circular)
  std::string ductType;         ///< duct type (Cylindrical/Rectangular)
  std::string shieldType;       ///< shielding type
  std::string shieldPenetrationType; ///< shielding penetration type (Cylindrical/Rectangular)
  double shieldPenetrationZOffset; ///< shield penetration z-offset
  double shieldPenetrationXOffset; ///< shield penetration x-offset
  double shieldPenetrationRadius; ///< penetration radius in the shield
  double shieldPenetrationWidth; ///< shielding rectangular penetration full width
  double shieldPenetrationHeight; ///< shielding rectangular penetration full height
  double shieldPenetrationTiltXmin; ///< shielding penetration xmin plane tilt angle
  double shieldLedgeThick;      ///< thickness of ledge above penetration
  double shieldLedgeLength;     ///< length of ledge above penetration (rectangular shield only)
  double shieldLedgeZOffset;    ///< z-offset of ledge above penetration
  int shieldLedgeActive;        ///< shield ledge active flag (rectangular shield only)
  double shieldThick;           ///< shielding thickness
  double shieldWidthRight;      ///< shielding right-width
  double shieldWidthLeft;       ///< shielding left-width
  double shieldDepth;           ///< shielding depth
  double shieldHeight;          ///< shielding height
  double shieldWallOffset;      ///< shield offset from the wall (along y)

  int voidMat;                  ///< Void material
  std::string shieldMat;        ///< shielding material

 public:

  DuctGenerator();
  DuctGenerator(const DuctGenerator&);
  DuctGenerator& operator=(const DuctGenerator&);
  virtual ~DuctGenerator();

  virtual void generate(FuncDataBase&,
			const std::string&,
			const std::string&,
			const std::string&,
			const std::string&,
			const double&,
			const double&,const double&,const double&) const;
  void setSize(const double&); // radius
  void setSize(const double&, const double&); // width x height

};

}

#endif
