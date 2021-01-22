/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/CleaningMagnetGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_CleaningMagnetGenerator_h
#define setVariable_CleaningMagnetGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CleaningMagnetGenerator
  \version 1.0
  \author Konstantin Batkov
  \date July 2020
  \brief CleaningMagnetGenerator for variables
*/

class CleaningMagnetGenerator
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double gap;                   ///< Void gap between cores

  double yokeLength;            ///< Yoke length
  double yokeDepth;             ///< Yoke depth
  double yokeThick;             ///< Yoke thick

  std::string mat;              ///< Core material
  std::string yokeMat;          ///< Yoke material
  std::string voidMat;          ///< Void material

 public:

  CleaningMagnetGenerator();
  CleaningMagnetGenerator(const CleaningMagnetGenerator&);
  CleaningMagnetGenerator& operator=(const CleaningMagnetGenerator&);
  virtual ~CleaningMagnetGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
