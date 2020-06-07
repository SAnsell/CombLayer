/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/TWCavityGenerator.h
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
#ifndef setVariable_TWCavityGenerator_h
#define setVariable_TWCavityGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TWCavityGenerator
  \version 1.0
  \author Konstantin Batkov
  \date June 2020
  \brief TWCavityGenerator for variables
*/

class TWCavityGenerator
{
 private:

  int nCells;                   ///< Number of regular cells
  double cellLength;            ///< Normal cell length
  double cellRadius;            ///< Normal cell inner radius
  double irisLength;            ///< Iris length
  double irisRadius;            ///< Iris radius
  double couplerLength;         ///< Coupler cell length
  double couplerWidth;          ///< Coupler cell width
  double wallThick;             ///< Wall thickness
  std::string wallMat;          ///< Wall material

 public:

  TWCavityGenerator();
  TWCavityGenerator(const TWCavityGenerator&);
  TWCavityGenerator& operator=(const TWCavityGenerator&);
  virtual ~TWCavityGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
