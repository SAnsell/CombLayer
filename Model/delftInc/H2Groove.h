/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/H2Groove.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef delftSystem_H2Groove_h
#define delftSystem_H2Groove_h

class Simulation;

namespace delftSystem
{

/*!
  \class H2Groove
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief H2Groove [insert object]
*/

class H2Groove : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:

  const int gIndex;            ///< groove index
  
  const int siIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  int face;                 ///< 0 not object / +/- Y direction
  double height;            ///< Height 
  double xyAngleA;          ///< angle A 
  double xyAngleB;          ///< Angle B

  double siTemp;           ///< Moderator temperature
  int siMat;                ///< Moderator material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createLinks();

 public:

  H2Groove(const std::string&,const int);
  H2Groove(const H2Groove&);
  H2Groove& operator=(const H2Groove&);
  ~H2Groove();

  /// Access main cell
  int getMainCell() const { return siIndex+1 ; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 
