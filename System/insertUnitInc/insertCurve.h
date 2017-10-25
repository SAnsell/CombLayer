/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnitInc/insertCurve.h
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
#ifndef insertSystem_insertCurve_h
#define insertSystem_insertCurve_h

class Simulation;

namespace insertSystem
{
/*!
  \class insertCurve
  \version 1.0
  \author S. Ansell
  \date February 2017
  \brief Cylinder inserted in object 
  
  Designed to be a quick plate to put an object into a model
  for fluxes/tallies etc
*/

class insertCurve : public insertSystem::insertObject
{
 private:

  int yFlag;                 ///< direction of centre from origin [+/-]
  double radius;             ///< Main radius
  double width;              ///< Full Width
  double height;             ///< Full Height
  double length;             ///< Radial length

  Geometry::Vec3D Centre;    ///< Rotation centre
  
  virtual void populate(const FuncDataBase&);
  void createUnitVector(const Geometry::Vec3D&,
			const attachSystem::FixedComp&);

  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createUnitVector(const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&);



  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void mainAll(Simulation&);

 public:

  insertCurve(const std::string&);
  insertCurve(const insertCurve&);
  insertCurve& operator=(const insertCurve&);
  virtual ~insertCurve();


  void setValues(const double,const double,const double,
		 const double,const int);
  void setValues(const double,const double,const double,
		 const double,const std::string&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const Geometry::Vec3D&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
