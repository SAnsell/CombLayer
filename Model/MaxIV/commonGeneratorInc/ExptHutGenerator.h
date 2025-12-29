/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/ExptHutGenerator.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
#ifndef setVariable_ExptHutGenerator_h
#define setVariable_ExptHutGenerator_h

class FuncDataBase;

namespace setVariable
{
  class XRayHutchBaseGenerator;
/*!
  \class ExptHutGenerator
  \version 1.1
  \author S. Ansell & K. Batkov
  \date December 2022
  \brief ExperimentalHutch variable generator
*/

class ExptHutGenerator  :
    public XRayHutchBaseGenerator
{
 private:

  double ringWidth;     ///< Ring side width (interior)
  double cornerAngle;   ///< Angle of corner
  double cornerYStep;   ///< Setp back for corner point

  double pbTiltedThick; ///< Thickness of lead in tilted wall
  double pbFrontThick;  ///< if front lead is needed (+ve)

  double fHoleRadius;   ///< Front hole radius
  double fHoleXStep;    ///< Front hole x-step
  double fHoleZStep;    ///< Front hole z-step

  double frontVoid;             ///< Extension for inner front void space
  double backVoid;              ///< Extension for inner back void space

 public:

  ExptHutGenerator();
  ExptHutGenerator(const ExptHutGenerator&) = default;
  ExptHutGenerator& operator=(const ExptHutGenerator&) = default;
  ~ExptHutGenerator() = default;

  void setFrontExt(const double T) { frontVoid=T; }
  void setBackExt(const double T) { backVoid=T; }

  void setCorner(const double,const double);

  void setFrontLead(const double T) { pbFrontThick=T; }
  void setFrontHole(const double,const double,const double);
  void setFloorShineThick(const double T){ floorShineThick=T; }
  void setFloorShineLength(const double T){ floorShineLength=T; }
  void generateHut(FuncDataBase&,const std::string&,const double,const double) const;

};

}

#endif
