/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/QuadUnitGenerator.h
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
#ifndef setVariable_QuadUnitGenerator_h
#define setVariable_QuadUnitGenerator_h

class FuncDataBase;


namespace setVariable
{

  class QuadrupoleGenerator;
  
/*!
  \class QuadUnitGenerator
  \version 1.0
  \author S. Ansell
  \date January 2019
  \brief QuadUnitGenerator for variables
*/

class QuadUnitGenerator 
{
 private:

  double length;                ///< frame length
  double inWidth;               ///< Inner width [flats]
  double ringWidth;             ///< Inner width [to divide not point]
  double outPointWidth;         ///< Outer [wall side] half-width
  double height;                ///< Inner height [straight]

  double endGap;                ///< Inner width
  double endLength;            ///< Inner height [straight]

  double wallThick;             ///< Wall thickness

  double flangeRadius;          ///< Joining Flange radius 
  double flangeLength;          ///< Joining Flange length
  
  std::string voidMat;                   ///< void material
  std::string wallMat;                   ///< wall material
  std::string flangeMat;                  ///< Pipe/flange material

  /// accessable generator
  std::unique_ptr<QuadrupoleGenerator> QGen;
  
 public:

  QuadUnitGenerator();
  QuadUnitGenerator(const QuadUnitGenerator&);
  QuadUnitGenerator& operator=(const QuadUnitGenerator&);
  virtual ~QuadUnitGenerator();
  
  
  virtual void generatePipe(FuncDataBase&,const std::string&,
			    const double) const;

};

}

#endif
 
