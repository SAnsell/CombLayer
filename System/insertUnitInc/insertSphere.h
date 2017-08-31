/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnitInc/insertSphere.h
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
#ifndef insertSystem_insertSphere_h
#define insertSystem_insertSphere_h

class Simulation;

namespace insertSystem
{
/*!
  \class insertSphere
  \version 1.0
  \author S. Ansell
  \date June 2016
  \brief Spherical insert object
  
  Designed to be a quick spher to put an object into a model
  for fluxes/tallies etc
*/

class insertSphere : public insertObject
{
 private:
  

  double radius;             ///< Full Width

  virtual void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  virtual void findObjects(Simulation&);

  void mainAll(Simulation&);

 public:

  insertSphere(const std::string&);
  insertSphere(const insertSphere&);
  insertSphere& operator=(const insertSphere&);
  virtual ~insertSphere();

  void setValues(const double,const int);
  void setValues(const double,const std::string&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(Simulation&,const Geometry::Vec3D&);

  
};

}

#endif
 
