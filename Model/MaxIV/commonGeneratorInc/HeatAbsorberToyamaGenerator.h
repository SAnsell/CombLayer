/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/HeatAbsorberToyamaGenerator.h
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
#ifndef setVariable_HeatAbsorberToyamaGenerator_h
#define setVariable_HeatAbsorberToyamaGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class HeatAbsorberToyamaGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2025
  \brief HeatAbsorberToyamaGenerator for variables
*/

class HeatAbsorberToyamaGenerator
{
 private:
  double width;                 ///< Width
  double height;                ///< Height
  double dumpWidth;            ///< dump width
  double dumpHeight;           ///< dump height
  double dumpLength;            ///< Full length of dump
  double dumpEndRadius;         ///< Dump end curvature radius
  double dumpFrontRadius;       ///< Curvature radius at dump front
  double dumpXOffset;           ///< x-offset of the dump penetration
  // beam penetration
  double gapWidth;            ///< penetration width
  double gapHeight;           ///< penetration height

  bool closed;                 ///< open/closed flag

  std::string mainMat;          ///< Main material
  std::string voidMat;          ///< Wall material

 public:

  HeatAbsorberToyamaGenerator();
  HeatAbsorberToyamaGenerator(const HeatAbsorberToyamaGenerator&);
  HeatAbsorberToyamaGenerator& operator=(const HeatAbsorberToyamaGenerator&);
  virtual ~HeatAbsorberToyamaGenerator();

  virtual void generate(FuncDataBase&,const std::string&,const double) const;

};

}

#endif
