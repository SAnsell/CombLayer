/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/LinacSexuGenerator.h
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
#ifndef setVariable_LinacSexuGenerator_h
#define setVariable_LinacSexuGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class LinacSexuGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief LinacSexuGenerator for variables
*/

class LinacSexuGenerator 
{
 private:

  double length;                ///< frame length

  double frameRadius;           ///< Gap to start of frame
  double frameOuter;            ///< Thikckness of frame

  double poleYAngle;            ///< Rotation of +X Pole about Y
  double poleGap;               ///< Gap from centre point
  double poleRadius;            ///< Radius of pole piece cut
  double poleWidth;             ///< width [in rotated frame]

  double coilRadius;            ///< Radius of coil start
  double coilWidth;             ///< Cross width of coil
  double coilInner;             ///< Inner plate width
  double coilBase;              ///< Coil base angle cut width
  double coilBaseDepth;         ///< Coil Base start height
  double coilAngle;             ///< Top Cut angle
  double coilEndExtra;          ///< Length of coil pieces
  double coilEndRadius;         ///< Coil extra radius [round]

  
  std::string poleMat;          ///< pole piece of magnet
  std::string coreMat;          ///< core of magnet 
  std::string coilMat;          ///< coil material
  std::string frameMat;         ///< Iron material

  
 public:

  LinacSexuGenerator();
  LinacSexuGenerator(const LinacSexuGenerator&);
  LinacSexuGenerator& operator=(const LinacSexuGenerator&);
  virtual ~LinacSexuGenerator();

  /// Angle in deg for main axis rotation
  void setPoleAngle(const double A) { poleYAngle=A; }
  void setRadius(const double,const double);
  virtual void generateSexu(FuncDataBase&,const std::string&,
			   const double) const;

};

}

#endif
 
