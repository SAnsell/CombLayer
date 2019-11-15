/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/BremOpticsCollGenerator.h
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double colYStep;         ///< absorber y offset
  double holeWidth;           ///< width of hole
  double holeHeight;          ///< height of hole
  double colLength;           ///< Length of absorber
  double colRadius;           ///< Absorber radius

  int extActive;        ///< true if external part should be built
  double extXStep;          ///< x offset of external part
  double extZStep;          ///< z offset of external part

  double extLength;            ///< Extent unit length
  double extRadius;            ///< Extent unit radius


  double pipeXSec;            ///< Pipe X/Z width
  double pipeYStep;           ///< Pipe step down block
  double pipeZStep;           ///< Pipe under step
  double pipeWidth;           ///< Centre-centre width
  double pipeMidGap;          ///< Gap between top halves

  std::string voidMat;                ///< void material
  std::string colMat;               ///< absorber material
  std::string wallMat;                ///< Fe material layer
  std::string waterMat;               ///< water cooling material
  std::string pipeMat;                ///< pipe outer material


 public:

  BremOpticsCollGenerator();
  BremOpticsCollGenerator(const BremOpticsCollGenerator&);
  BremOpticsCollGenerator& operator=(const BremOpticsCollGenerator&);
  ~BremOpticsCollGenerator();

  template<typename CF> void setCF();

  void setAperature(const double,const double);
  void setMaterial(const std::string&,const std::string&);

  void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
