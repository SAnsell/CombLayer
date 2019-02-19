/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/QuadrupoleGenerator.h
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
#ifndef setVariable_QuadrupoleGenerator_h
#define setVariable_QuadrupoleGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class QuadrupoleGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief QuadrupoleGenerator for variables
*/

class QuadrupoleGenerator 
{
 private:

  double vertGap;               ///< Inner vertical space

  double width;                 ///< horrizontal width [inner]
  double height;                ///< vertical width [inner]

  double coilLength;            ///< Length of coil
  double coilCornerRad;         ///< Corner radius of coil
  double coilWidth;             ///< Width of coil

  double frameThick;            ///< Thikckness of frame

  double poleLength;            ///< Length in y axis
  double poleRadius;            ///< Pole piece radius
  double poleZStep;             ///< Step down/up [NON-rotated frame]
  double poleYAngle;            ///< Rotation about Y axis
  double poleStep;              ///< Step down/up [rotated frame
  double poleWidth;             ///< width [in rotated frame]

  std::string poleMat;          ///< pole piece of magnet
  std::string coreMat;          ///< core of magnet 
  std::string coilMat;          ///< coil material
  std::string frameMat;         ///< Iron material

  
 public:

  QuadrupoleGenerator();
  QuadrupoleGenerator(const QuadrupoleGenerator&);
  QuadrupoleGenerator& operator=(const QuadrupoleGenerator&);
  virtual ~QuadrupoleGenerator();
  
  
  virtual void generateQuad(FuncDataBase&,const std::string&,
			    const double,const double) const;

};

}

#endif
 
