/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/WendiGenerator.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef setVariable_WendiGenerator_h
#define setVariable_WendiGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class WendiGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2024
  \brief WendiGenerator for variables
*/

class WendiGenerator
{
 private:

  double radius;                ///< Moderator radius
  double cRadius;               ///< Counter tube outer radius
  double height;                ///< Height
  double wRadius;               ///< Tungsten layer inner radius
  double wThick;                ///< Tungsten layer thickness
  double wHeight;               ///< Tungsten layer full height (with support)
  double wOffset;               ///< Tungsten layer vertical offset from the bottom surface
  double cOffset;               ///< Counter tube vertical offset from Tungsten
  double heRadius;              ///< Active volume inner radius
  double heWallThick;           ///< Counter hull wall thickness
  double heHeight;              ///< Active volume height
  double rubberThick;           ///< Borated rubber patch thickness
  double bottomInsulatorRadius; ///< Bottom insulator radius
  double bottomInsulatorHeight; ///< Bottom insulator height
  double topInsulatorRadius;    ///< Top insulator radius
  double topInsulatorHeight;    ///< Top insulator height

  std::string modMat;           ///< Moderator material
  std::string wMat;             ///< Tungsten powder material
  std::string heWallMat;        ///< Active volume wall material
  std::string heMat;            ///< Active volume material
  std::string rubberMat;        ///< Borated rubber material

 public:

  WendiGenerator();
  WendiGenerator(const WendiGenerator&);
  WendiGenerator& operator=(const WendiGenerator&);
  virtual ~WendiGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
