/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/insertPlate.h
*
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef ModelSupport_insertPlate_h
#define ModelSupport_insertPlate_h

class Simulation;

namespace ModelSupport
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

class insertPlate : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int ptIndex;             ///< Index of surface offset
  int cellIndex;                 ///< Cell index
  int populated;                 ///< 1:var

  double zAngle;            ///< Z angle rotation
  double xyAngle;           ///< XY angle rotation

  double width;             ///< Full Width
  double height;            ///< Full Height
  double depth;             ///< Full Depth 

  int defMat;               ///< Material

  void populate(const FuncDataBase&);
  void createUnitVector(const Geometry::Vec3D&,
			const attachSystem::FixedComp&);

  void createUnitVector(const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&,
			const Geometry::Vec3D&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void findObjects(const Simulation&);

  void mainAll(Simulation&);

 public:

  insertPlate(const std::string&);
  insertPlate(const insertPlate&);
  insertPlate& operator=(const insertPlate&);
  ~insertPlate();

  void setValues(const double,const double,const double,
		 const int);
  void setAngles(const double,const double);
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&);

};

}

#endif
 
