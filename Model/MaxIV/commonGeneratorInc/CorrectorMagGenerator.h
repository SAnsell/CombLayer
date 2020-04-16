/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/CorrectorMagGenerator.h
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
#ifndef setVariable_CorrectorMagGenerator_h
#define setVariable_CorrectorMagGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CorrectorMagGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief CorrectorMagGenerator for variables
*/

class CorrectorMagGenerator 
{
 private:

  double length;                ///< frame length

  double magOffset;             ///< Magnet offset from pipe centre
  double magHeight;             ///< Magnet height
  double magWidth;              ///< Magnet segment width 
  double magLength;             ///< Magnet full length
  double magCorner;             ///< Magnet corner radius
  double magInnerWidth;         ///< Magnet inner width
  double magInnerLength;        ///< Magnet inner length

  double frameHeight;            ///< height of plate

  std::string voidMat;          ///< void material
  std::string coilMat;          ///< coil material
  std::string frameMat;         ///< Iron material


 public:

  CorrectorMagGenerator();
  CorrectorMagGenerator(const CorrectorMagGenerator&);
  CorrectorMagGenerator& operator=(const CorrectorMagGenerator&);
  virtual ~CorrectorMagGenerator();

  /// Angle in deg for main axis rotation
  virtual void generateMag(FuncDataBase&,const std::string&,
			   const double,const bool) const;

};

}

#endif
 
