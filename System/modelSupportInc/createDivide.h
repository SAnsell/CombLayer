/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/createDivide.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ModelSupport_createDivide_h
#define ModelSupport_createDivide_h

namespace ModelSupport
{

  Geometry::Plane* 
    createDividerSurf(const int,const Geometry::Vec3D&,
		      const Geometry::Plane*,const Geometry::Plane*,
		      const Geometry::Plane*,const Geometry::Plane*);
}


#endif
