/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/YagUnitBigGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell / Konstantin Batkov
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
#ifndef setVariable_YagUnitBigGenerator_h
#define setVariable_YagUnitBigGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class YagUnitBigGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief YagUnitBigGenerator for variables
*/

class YagUnitBigGenerator
{
 private:

  double radius;               ///< void radius
  double height;               ///< void height [+z]
  double depth;                ///< void depth [-z]
  double wallThick;            ///< pipe thickness

  double flangeRadius;         ///< Joining Flange radius
  double flangeLength;         ///< Joining Flange length
  double plateThick;           ///< flange plate thick

  // centre port [left]
  double viewAZStep;            ///< Viewing Z-lift
  double viewARadius;           ///< Viewing Radius
  double viewAThick;            ///< Wall thickness
  double viewALength;           ///< viewing Length [centre to flange end]
  double viewAFlangeRadius;     ///< ViewA Flange radius
  double viewAFlangeLength;     ///< Joining Flange length
  double viewAPlateThick;       ///< Cover plate thickness

  // side view port [back]
  double viewBYStep;            ///< Step from origin
  double viewBRadius;           ///< Viewing Radius
  double viewBThick;            ///< Wall thickness
  double viewBLength;           ///< viewing Length [centre to flange end]
  double viewBFlangeRadius;     ///< Flange outer Radius
  double viewBFlangeLength;     ///< Flange length
  double viewBPlateThick;       ///< View Flange Plate thickness

  // front/back port
  double portRadius;         ///< port Radius
  double portThick;          ///< port wall thickness
  double portFlangeRadius;   ///< port flange Radius
  double portFlangeLength;   ///< port flange length
  double frontLength;        ///< front Length [centre to flange end]
  double frontILen;          ///< front Length [centre to start]
  double backLength;         ///< back Length [centre to flange end]
  double backILen;           ///< front Length [centre to start]

  double outerRadius;        ///< Radius to build simple tower void

  std::string voidMat;               ///< void material
  std::string mainMat;               ///< wall material

 public:

  YagUnitBigGenerator();
  YagUnitBigGenerator(const YagUnitBigGenerator&);
  YagUnitBigGenerator& operator=(const YagUnitBigGenerator&);
  virtual ~YagUnitBigGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();
  template<typename T> void setPortCF();
  template<typename T> void setViewACF();

  double getPortRadius() const {return portRadius;}

  void generateYagUnit(FuncDataBase&,
		       const std::string&) const;

};

}

#endif
