/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/HeimdalCaveGenerator.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef setVariable_HeimdalCaveGenerator_h
#define setVariable_HeimdalCaveGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class HeimdalCaveGenerator
  \version 1.0
  \author S. Ansell
  \date January 2024
  \brief HeimdalCaveGenerator for varialbes
*/

class HeimdalCaveGenerator
{
 private:     
  
  double length;                 ///< inner beam length
  double height;                 ///< full width (no sub floor)
  double width;                  ///< inner width (void)
  double corridorWidth;          ///< width of corridor (void)
  double corridorLength;         ///< length of corridor (void)
  double corridorTLen;           ///< length of thick part of corridor
  double doorLength;             ///< length of door on corridor

  double roofGap;                ///< width of roof gap
  double beamWidth;              ///< beam width (front gap)
  double beamHeight;             ///< beam height (front gap)

  double midZStep;               ///< mid level
  double midThick;               ///< mid thickness
  double midHoleRadius;          ///< mid level Hole
  
  double mainThick;             ///< main thickness (inc roof)
  double subThick;              ///< thickness of thin walls
  double extGap;                ///< extension gap thickness for gap covering

  std::string wallMat;
  std::string voidMat;
  
 public:

  HeimdalCaveGenerator();
  HeimdalCaveGenerator(const HeimdalCaveGenerator&);
  HeimdalCaveGenerator& operator=(const HeimdalCaveGenerator&);
  ~HeimdalCaveGenerator();

  
  void generateCave(FuncDataBase&,const std::string&) const;

};

}

#endif
 
