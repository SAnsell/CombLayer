/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/SpecialSurf.h
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
#ifndef ModelSupport_SpecialSurf_h
#define ModelSupport_SpecialSurf_h

namespace ModelSupport
{

int addPlane(const int,const Geometry::Vec3D&,
	     const int,const int,const int,const int);
int addPlane(const int,const Geometry::Vec3D&,
	     const int,const int,const int);
int addPlane(const int,const Geometry::Vec3D&,
	     const Geometry::Vec3D&);
 
size_t calcVertex(const int,const int,const int,
		std::vector<Geometry::Vec3D>&,const int);
size_t calcVertex(const int,const int,const int,
		  Geometry::Vec3D&,const Geometry::Vec3D&);

void listSurfaces();

int checkSurface(const int,const Geometry::Vec3D&);

}


#endif
