/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/surfEqual.h
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
#ifndef ModelSupport_surfEqual_h
#define ModelSupport_surfEqual_h

namespace ModelSupport
{

const Geometry::Surface* equalSurface(const Geometry::Surface*);
Geometry::Surface* equalSurface(Geometry::Surface*);
 
template<typename SurfType,typename RetType>
RetType EqualSurf(SurfType,const std::map<int,Geometry::Surface*>&);

int
equalSurfNum(const Geometry::Surface*);

bool cmpSurfaces(const Geometry::Surface*,const Geometry::Surface*);
bool oppositeSurfaces(const Geometry::Surface*,const Geometry::Surface*);

}

#endif
