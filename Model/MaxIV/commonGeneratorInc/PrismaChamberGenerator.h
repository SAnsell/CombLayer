/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/PrismaChamberGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef setVariable_PrismaChamberGenerator_h
#define setVariable_PrismaChamberGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PrismaChamberGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief PrismaChamberGenerator for variables
*/

class PrismaChamberGenerator 
{
 private:
  

  double radius;                ///< void radius
  double beamRadius;            ///< radius of Y (Beam) port
  double sideRadius;            ///< radiu of side ports

  double wallThick;             ///< pipe thickness

  double height;                ///< full to flange height above beam
  double depth;                 ///< full to flange depth below beam

  double beamXStep;             ///< beam offset [x directoin
  double frontLength;           ///< full to flange length (-ve Y)
  double backLength;            ///< full to flange length
  double sideLength;            ///< full to flange length

  double flangeRadius;          ///< Top flange radius
  double flangeXRadius;         ///< flange cross direction
  double flangeBeamRadius;      ///< Joining Flange beam direct

  double flangeLength;          ///< Top flange radius
  double flangeXLength;         ///< flange cross direction
  double flangeBeamLength;      ///< Joining Flange beam direct

  double sideThick;             ///< Side thickness
  double plateThick;            ///< Joining Flange radius

  std::string voidMat;                  ///< void material
  std::string wallMat;                  ///< main material
  std::string plateMat;                 ///< plate material

 public:

  PrismaChamberGenerator();
  PrismaChamberGenerator(const PrismaChamberGenerator&);
  PrismaChamberGenerator& operator=(const PrismaChamberGenerator&);
  virtual ~PrismaChamberGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();

  void generateChamber(FuncDataBase&,
		       const std::string&) const;

};

}

#endif
 
