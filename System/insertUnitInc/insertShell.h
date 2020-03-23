/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnitInc/insertShell.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef insertSystem_insertShell_h
#define insertSystem_insertShell_h

class Simulation;

namespace insertSystem
{
/*!
  \class insertShell
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief Spherical shell [no inner] 
  
  Designed to be a quick shell to put an object into a model
  for fluxes/tallies etc
*/

class insertShell : public insertObject
{
 private:
  

  double innerRadius;             ///< void in middle
  double outerRadius;             ///< outer Radius

  int innerMat;                   ///< inner material -ve for not built
  
  virtual void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  virtual void findObjects(Simulation&);

  void mainAll(Simulation&);

 public:

  insertShell(const std::string&);
  insertShell(const std::string&,const std::string&);
  insertShell(const insertShell&);
  insertShell& operator=(const insertShell&);
  virtual ~insertShell();

  void setValues(const double,const double,const int,const int);
  void setValues(const double,const double,const std::string&,
		 const std::string&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(Simulation&,const Geometry::Vec3D&);

  
};

}

#endif
 
