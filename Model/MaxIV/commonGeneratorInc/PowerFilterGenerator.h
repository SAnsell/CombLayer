/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/PowerFilterGenerator.h
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
#ifndef setVariable_PowerFilterGenerator_h
#define setVariable_PowerFilterGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PowerFilterGenerator
  \version 1.0
  \author Konstantin Batkov
  \date May 2025
  \brief PowerFilterGenerator for variables
*/

class PowerFilterGenerator
{
 private:

  double maxLength;                ///< Total maxLength including void
    double baseLength;            ///< length of base (parameter a)
    double wedgeAngle;            ///< angle of the wedge
  double width;                 ///< Width
  double height;                ///< Height
  double baseHeight;             ///< Wall thickness
    double filterZOffset;         ///< z-offset of the single filter blade

  std::string mat;          ///< Main material
  std::string wallMat;          ///< Wall material

 public:

  PowerFilterGenerator();
  PowerFilterGenerator(const PowerFilterGenerator&);
  PowerFilterGenerator& operator=(const PowerFilterGenerator&);
  virtual ~PowerFilterGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
