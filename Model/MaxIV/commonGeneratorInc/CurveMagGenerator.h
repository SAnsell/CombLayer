/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/CurveMagGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_CurveMagGenerator_h
#define setVariable_CurveMagGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CurveMagGenerator
  \version 1.0
  \author Stuart Ansell
  \date August 2020
  \brief CurveMagGenerator for variables
*/

class CurveMagGenerator
{
 private:

  double poleGap;               ///< Gap on pole
  double poleHeight;            ///< Gap height on pole
  double poleAngle;             ///< Angle on pole pieces cone

  double coilGap;               ///< gap between coils
  double coilArcRadius;         ///< radius of main curve
  double coilArcLength;         ///< length of main curve
  double coilDepth;             ///< depth across coil
  double coilWidth;             ///< Width of coil

  std::string poleMat;                  ///< Pole material
  std::string coilMat;                  ///< Coil material

 public:

  CurveMagGenerator();
  CurveMagGenerator(const CurveMagGenerator&);
  CurveMagGenerator& operator=(const CurveMagGenerator&);
  virtual ~CurveMagGenerator();

  virtual void generateMag(FuncDataBase&,const std::string&) const;

};

}

#endif
