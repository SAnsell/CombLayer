/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGeneratorInc/CollUnitGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_CollUnitGenerator_h
#define setVariable_CollUnitGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CollUnitGenerator
  \version 1.0
  \author Stuart Ansell
  \date November 2021
  \brief CollUnitGenerator for variables
*/

class CollUnitGenerator 
{
 private:

  double tubeRadius;            ///< radius of tubing
  double tubeWall;              ///< wall thickness of tubing

  double tubeYStep;             ///< y-forward step
  double tubeTopGap;            ///< Gap at the top (free area)
  double tubeMainGap;           ///< Gap in the plate area
  double tubeTopLength;         ///< Length up from the centre point
  double tubeDownLength;        ///< Length down from the centre point

  double plateThick;            ///< Thickness of plate
  double plateLength;           ///< Length of plate
  double plateWidth;            ///< Width of plate

  size_t nHoles;                ///< number of holes
  double holeGap;               ///< Gap between hole
  double holeRadius;            ///< Radius of hols
    
  std::string voidMat;          ///< void material
  std::string plateMat;        ///< screen holder material
  std::string pipeMat;          ///< pipe material
  std::string waterMat;         ///< water material

 public:

  CollUnitGenerator();
  CollUnitGenerator(const CollUnitGenerator&);
  CollUnitGenerator& operator=(const CollUnitGenerator&);
  virtual ~CollUnitGenerator();


  virtual void generateScreen(FuncDataBase&,const std::string&) const;
  
};

}

#endif
