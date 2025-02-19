/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/MovableSafetyMaskGenerator.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef setVariable_MovableSafetyMaskGenerator_h
#define setVariable_MovableSafetyMaskGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MovableSafetyMaskGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2025
  \brief MovableSafetyMaskGenerator for variables
*/

class MovableSafetyMaskGenerator
{
 private:
  double width;                 ///< Width
  double height;                ///< Height
  double uMaskWidth;          ///< Undulator mask width
  double uMaskHeight;           ///< Undulator mask height
  double wMaskWidth;            ///< Wiggler mask width
  double wMaskHeight;           ///< Wiggler mask height

  std::string mainMat;          ///< Main material
  std::string voidMat;          ///< Wall material

 public:

  MovableSafetyMaskGenerator();
  MovableSafetyMaskGenerator(const MovableSafetyMaskGenerator&);
  MovableSafetyMaskGenerator& operator=(const MovableSafetyMaskGenerator&);
  virtual ~MovableSafetyMaskGenerator();

  virtual void generate(FuncDataBase&,const std::string&,const double) const;

};

}

#endif
