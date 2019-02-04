/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/DipoleChamberGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_DipoleChamberGenerator_h
#define setVariable_DipoleChamberGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DipoleChamberGenerator
  \version 1.0
  \author S. Ansell
  \date January 2019
  \brief DipoleChamberGenerator for variables
*/

class DipoleChamberGenerator 
{
 private:

  double length;                ///< frame length
  double outWidth;              ///< Step out side
  double ringWidth;             ///< Step ring side at start
  double curveRadius;           ///< Radius of ring side curve
  double curveAngle;            ///< Total angle [deb]
  double height;                ///< Inner height [straight]

  double exitWidth;              ///< Cut in exit channel
  double exitHeight;             ///< Cut in exit channel
  double exitLength;             ///< Length of exit channel

  double flangeRadius;          ///< Joining Flange radius
  double flangeLength;          ///< Joining Flange length

  double wallThick;             ///< Wall thickness
  
  double innerXFlat;            ///< First X flat inside
  double innerXOut;             ///< First X out side

  double elecXFlat;             ///< Flat cut
  double elecXCut;              ///< Curve on phon side
  double elecXFull;             ///< Curve on electron side

  std::string voidMat;                   ///< void material
  std::string wallMat;                   ///< wall material
  std::string innerMat;                   ///< wall material
  std::string flangeMat;                  ///< Pipe/flange material
 
 public:

  DipoleChamberGenerator();
  DipoleChamberGenerator(const DipoleChamberGenerator&);
  DipoleChamberGenerator& operator=(const DipoleChamberGenerator&);
  virtual ~DipoleChamberGenerator();
  
  
  virtual void generatePipe(FuncDataBase&,const std::string&,
			    const double) const;

};

}

#endif
 
