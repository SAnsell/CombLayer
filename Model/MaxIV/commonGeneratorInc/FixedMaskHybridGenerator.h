/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/FixedMaskHybridGenerator.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef setVariable_FixedMaskHybridGenerator_h
#define setVariable_FixedMaskHybridGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class FixedMaskHybridGenerator
  \version 1.0
  \author Konstantin Batkov
  \date December 2025
  \brief FixedMaskHybridGenerator for variables
*/

class FixedMaskHybridGenerator
{
 private:

  double length;                ///< Total length
  double radius;                ///< Outer radius

  double flangeLength;          ///< Flange length
  double flangeRadius;          ///< Flange radius
  double flangeGrooveLength;    ///< Start/end flange groove length
  double inAngle;               ///< Entrance aperture full angle
  double inRadius;              ///< Entrance aperture max radius
  double coneLength;            ///< Entrance conic aperture length
  double outWidth;              ///< Exit aperture min full width
  double outHeight;             ///< Exit aperture min full height
  double outAngle;              ///< Exit aperture full angle
  double outStraightLength;     ///< Exit aperture straight segment length

  std::string mat;              ///< FM material
  std::string flangeMat;        ///< Flange material

 public:

  FixedMaskHybridGenerator();
  FixedMaskHybridGenerator(const FixedMaskHybridGenerator&);
  FixedMaskHybridGenerator& operator=(const FixedMaskHybridGenerator&);
  virtual ~FixedMaskHybridGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
