/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/addInsertObj.h
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
#ifndef constructSystem_addInsertObj_h
#define constructSystem_addInsertObj_h

class Simulation;

namespace constructSystem
{

void
addInsertCurveCell(Simulation&,const std::string&,
		   const Geometry::Vec3D&,const Geometry::Vec3D&,
		   const int,const Geometry::Vec3D&,
		   const double,const double,const double,
		   const std::string&);
void
addInsertCylinderCell(Simulation&,const std::string&,
		      const std::string&,const std::string&,
		      const Geometry::Vec3D&,const double,
		      const double,const std::string&);
void
addInsertCylinderCell(Simulation&,const std::string&,
		      const Geometry::Vec3D&,const Geometry::Vec3D&,
		      const double,const double,const std::string&);
void
addInsertPlateCell(Simulation&,const std::string&,
		   const std::string&,const std::string&,
		   const Geometry::Vec3D&,const double,
		   const double,const double,const std::string&);
void
addInsertPlateCell(Simulation&,const std::string&,
		   const Geometry::Vec3D&,const Geometry::Vec3D&,
		   const Geometry::Vec3D&,const double,
		   const double,const double,const std::string&);
void
addInsertSphereCell(Simulation&,const std::string&,
		    const std::string&,const std::string&,
		    const Geometry::Vec3D&,const double,
		    const std::string&);
void
addInsertSphereCell(Simulation&,const std::string&,
		   const Geometry::Vec3D&,const double,
		   const std::string&);

void
addInsertGridCell(Simulation&,const std::string&,
		  const std::string&,const std::string&,
		  const Geometry::Vec3D&,
		  const size_t,const double,
		  const double,const double,
		  const std::string&);

void
addInsertGridCell(Simulation&,const std::string&,
		  const Geometry::Vec3D&,const Geometry::Vec3D&,
		  const Geometry::Vec3D&,
		  const size_t,const double,
		  const double,const double,
		  const std::string&);

 
}

#endif
 
