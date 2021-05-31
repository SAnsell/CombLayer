/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/LocalShieldingGenerator.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef setVariable_LocalShieldingGenerator_h
#define setVariable_LocalShieldingGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class LocalShieldingGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2021
  \brief LocalShieldingGenerator for variables
*/

class LocalShieldingGenerator
{
 private:

  double length;                ///< Total length
  double width;                 ///< Total width
  double height;                ///< Total height
  double zStep;                 ///< z offset
  double midHoleWidth;          ///< Width of the beam pipe penetration
  double midHoleHeight;         ///< Height of the beam pipe penetration
  double cornerWidth;           ///< Corner cut width
  double cornerHeight;          ///< Corner cut height

  std::string mainMat;          ///< Main material
  std::string cType;            ///< Corner type
  std::string opt;              ///< Option

 public:

  LocalShieldingGenerator();
  LocalShieldingGenerator(const LocalShieldingGenerator&);
  LocalShieldingGenerator& operator=(const LocalShieldingGenerator&);
  virtual ~LocalShieldingGenerator();

  void setSize(const double, const double, const double);
  void setMidHole(const double, const double);
  void setCorner(const double, const double, const std::string type="both");

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
