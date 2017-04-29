/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/insertPlate.h
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
#ifndef constructSystem_insertPlate_h
#define constructSystem_insertPlate_h

class Simulation;

namespace constructSystem
{
/*!
  \class insertPlate
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief Plate inserted in object 
  
  Designed to be a quick plate to put an object into a model
  for fluxes/tallies etc
*/

class insertPlate : public constructSystem::insertObject
{
 private:
    
  double width;             ///< Full Width
  double height;            ///< Full Height
  double depth;             ///< Full Depth 
  
  virtual void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void mainAll(Simulation&);

 public:

  insertPlate(const std::string&);
  insertPlate(const insertPlate&);
  insertPlate& operator=(const insertPlate&);
  virtual ~insertPlate();

  void setValues(const double,const double,const double,
		 const int);
  void setValues(const double,const double,const double,
		 const std::string&);
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const Geometry::Vec3D&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
