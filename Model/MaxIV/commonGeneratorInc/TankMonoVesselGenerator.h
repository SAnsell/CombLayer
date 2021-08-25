/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   speciesInc/TankMonoVesselGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_TankMonoVesselGenerator_h
#define setVariable_TankMonoVesselGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TankMonoVesselGenerator
  \version 1.0
  \author S. Ansell
  \date August 2018
  \brief TankMonoVesselGenerator for variables
*/

class TankMonoVesselGenerator : public VacBoxGenerator
{
 private:

  double baseGap;             ///< Extra at base for curve
  double topGap;               ///< Extra at top for curve
  
  double lidOffset;             ///< Lid flange offset from top of cylinder
  double lidRadius;             ///< Lid flange radius
  double lidDepth;              ///< Lid flange depth
   
 public:

  TankMonoVesselGenerator();
  TankMonoVesselGenerator(const TankMonoVesselGenerator&);
  TankMonoVesselGenerator& operator=(const TankMonoVesselGenerator&);
  virtual ~TankMonoVesselGenerator();

  void setLid(const double,const double,const double);
  
  virtual void generateBox(FuncDataBase&,const std::string&,
			   const double,const double,
			   const double) const;

};

}

#endif
 
