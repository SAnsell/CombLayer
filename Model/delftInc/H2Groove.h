/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/H2Groove.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

class H2Groove :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap
{
 private:
  
  int gID;                  ///< Groove ide
  
  double height;            ///< Height 
  double xyAngleA;          ///< angle A 
  double xyAngleB;          ///< Angle B

  double siTemp;           ///< Moderator temperature
  int siMat;                ///< Moderator material

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  H2Groove(const std::string&,const int);
  H2Groove(const H2Groove&);
  H2Groove& operator=(const H2Groove&);
  ~H2Groove() override;
  
  
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
