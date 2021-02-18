/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/DCMTankGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_DCMTankGenerator_h
#define setVariable_DCMTankGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DCMTankGenerator
  \version 1.0
  \author S. Ansell
  \date October 2019
  \brief DCMTankGenerator for variables
*/

class DCMTankGenerator : public VacBoxGenerator
{
 private:
  
  double baseThick;            ///< thickness of base
  double baseWidth;            ///< width of base
  double baseLength;           ///< length of base [beam]

  double topRadius;            ///< Radius of top dome
  double topLift;              ///< Lift of top dome

  
 public:

  DCMTankGenerator();
  DCMTankGenerator(const DCMTankGenerator&);
  DCMTankGenerator& operator=(const DCMTankGenerator&);
  virtual ~DCMTankGenerator();

  virtual void generateBox(FuncDataBase&,const std::string&,
			   const double,const double,const double,
			   const double) const;

};

}

#endif
 
