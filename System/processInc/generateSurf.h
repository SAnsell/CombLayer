/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/generateSurf.h
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
#ifndef ModelSupport_generateSurf_h
#define ModelSupport_generateSurf_h

namespace ModelSupport
{

Geometry::Plane*
buildRotatedPlane(surfRegister&,const int,
		  const Geometry::Plane*,
		  const double,
		  const Geometry::Vec3D&,
		  const Geometry::Vec3D&);

Geometry::Plane*
buildSignedShiftedPlane(surfRegister&,const int,const int,
			const Geometry::Plane*,
			const double);
Geometry::Plane*
buildShiftedPlane(surfRegister&,const int,
		  const Geometry::Plane*,
		  const double);
Geometry::Plane*
buildShiftedPlaneReversed(surfRegister&,const int,
			  const Geometry::Plane*,
			  const double);

Geometry::Plane*
buildPlaneRotAxis(surfRegister&,const int,
		  const Geometry::Vec3D&,const Geometry::Vec3D&,
		  const Geometry::Vec3D&,const double);

Geometry::Plane* 
buildPlane(surfRegister&,const int,const Geometry::Vec3D&,
	   const Geometry::Vec3D&,const Geometry::Vec3D&,
	   const Geometry::Vec3D&);

Geometry::Plane* 
buildPlane(surfRegister&,const int,const Geometry::Vec3D&,
	   const Geometry::Vec3D&);

Geometry::Plane* 
buildPlane(surfRegister&,const int,const Geometry::Vec3D&,
	   const double&);

Geometry::Cylinder* 
buildCylinder(surfRegister&,const int,const Geometry::Vec3D&,
	      const Geometry::Vec3D&,const double);

Geometry::Sphere* 
buildSphere(surfRegister&,const int,const Geometry::Vec3D&,const double);

Geometry::Cone* 
buildCone(surfRegister&,const int,const Geometry::Vec3D&,
	  const Geometry::Vec3D&,const Geometry::Vec3D&,
	  const Geometry::Vec3D&);

Geometry::Cone* 
buildCone(surfRegister&,const int,const Geometry::Vec3D&,
	  const Geometry::Vec3D&,const double);

Geometry::Cone* 
buildCone(surfRegister&,const int,const Geometry::Vec3D&,
	  const Geometry::Vec3D&,const double,const int);

Geometry::EllipticCyl* 
buildEllipticCyl(surfRegister&,const int,const Geometry::Vec3D&,
		 const Geometry::Vec3D&,
		 const Geometry::Vec3D&,const double,
		 const double);

}

#endif
