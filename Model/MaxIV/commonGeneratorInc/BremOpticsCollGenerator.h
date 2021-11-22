/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/BremOpticsCollGenerator.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef setVariable_BremOpticsCollGenerator_h
#define setVariable_BremOpticsCollGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BremOpticsCollGenerator
  \version 1.0
  \author K. Batkov
  \date Nov 2019
  \brief BremOpticsCollGenerator for variables
*/

class BremOpticsCollGenerator
{
 private:


  double extWidth;               ///< Width of W block
  double extHeight;              ///< Height of W block
  double length;              ///< Main length
  double wallThick;           ///< wall thickness of main tube

  double innerRadius;          ///< Inner radius of hole
  double flangeARadius;        ///< Flange A outer radius
  double flangeAInnerRadius;   ///< Flange A inner radius
  double flangeALength;        ///< Flange A length
  double flangeBRadius;        ///< Flange B outer radius
  double flangeBInnerRadius;        ///< Flange B outer radius
  double flangeBLength;        ///< Flange B length

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double holeWidth;           ///< width of hole
  double holeHeight;          ///< height of hole
  double colYStep;         ///< absorber y offset
  double colLength;           ///< Length of absorber
  double colRadius;           ///< Absorber radius

  int extActive;        ///< true if external part should be built
  double extXStep;          ///< x offset of external part
  double extZStep;          ///< z offset of external part

  double extLength;            ///< Extent unit length
  double extRadius;            ///< Extent unit radius

  std::string voidMat;                ///< void material
  std::string colMat;               ///< absorber material
  std::string wallMat;                ///< Fe material layer

 public:

  BremOpticsCollGenerator();
  BremOpticsCollGenerator(const BremOpticsCollGenerator&);
  BremOpticsCollGenerator& operator=(const BremOpticsCollGenerator&);
  ~BremOpticsCollGenerator();

  template<typename CF> void setCF();

  void setAperture(const double,const double);
  void setMaterial(const std::string&,const std::string&);
  void setFlangeInnerRadius(const double,const double);

  void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
