/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/ProximityShieldingGenerator.h
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
#ifndef setVariable_ProximityShieldingGenerator_h
#define setVariable_ProximityShieldingGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ProximityShieldingGenerator
  \version 1.0
  \author Konstantin Batkov
  \date March 2025
  \brief ProximityShieldingGenerator for variables
*/

class ProximityShieldingGenerator
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double pipeRadius;             ///< Wall thickness

  std::string voidMat;          ///< Main material
  std::string wallMat;          ///< Wall material

 public:

  ProximityShieldingGenerator();
  ProximityShieldingGenerator(const ProximityShieldingGenerator&);
  ProximityShieldingGenerator& operator=(const ProximityShieldingGenerator&);
  virtual ~ProximityShieldingGenerator();

  virtual void generate(FuncDataBase&,const std::string&,const double) const;

};

}

#endif
