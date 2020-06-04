/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/TDCCavityGenerator.h
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
#ifndef setVariable_TDCCavityGenerator_h
#define setVariable_TDCCavityGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TDCCavityGenerator
  \version 1.0
  \author Konstantin Batkov
  \date June 2020
  \brief TDCCavityGenerator for variables
*/

class TDCCavityGenerator
{
 private:

  double cellLength;            ///< Normal cell length
  double irisLength;            ///< Iris length
  double radius;                ///< Wall inner radius
  double innerRadius;           ///< Pipe inner radius
  double wallThick;             ///< Wall thickness

  int nCells;                   ///< Number of regular cells
  std::string wallMat;          ///< Wall material
  double couplerThick;          ///< coupler cell thickness
  double couplerWidth;          ///< coupler cell width

 public:

  TDCCavityGenerator();
  TDCCavityGenerator(const TDCCavityGenerator&);
  TDCCavityGenerator& operator=(const TDCCavityGenerator&);
  virtual ~TDCCavityGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
