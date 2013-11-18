/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/surfVector.h
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
#ifndef Geometry_surfVector_h
#define Geometry_surfVector_h

namespace Geometry
{
  /// Typedef of all exported classes 
  typedef boost::mpl::vector<
    Geometry::ArbPoly,Geometry::Cone,
    Geometry::CylCan,Geometry::Cylinder,
    Geometry::EllipticCyl,
    Geometry::General,Geometry::MBrect,
    Geometry::NullSurface,Geometry::Plane,
    Geometry::Sphere,Geometry::Torus>::type ExportClass;

  /// String names of all exported classes 
  static const char* ExportNames[] =
    {
      "ArbPoly","Cone","CylCan","Cylinder",
      "EllipticCyl","General","MBrect","NullSurface",
      "Plane","Sphere",
      "Torus"  };

  /// Number of non-abstract surface classes
  static const int ExportSize(boost::mpl::size<ExportClass>::value);

  /// This si a list of sort names (all)
  static const char* ShortNames[] =
    {
      "arb",0,
      "k/x","k/y","k/z","kx","ky","kz",0,
      "rcc",0,
      "c/x","c/y","c/z","cx","cy","cz",0,
      "e/x","e/y","e/z","ex","ey","ez",0,
      "gq","sq",0,
      "box","rpp",0,
      "null",0,
      "p","px","py","pz",0,
      "so","s","sx","sy"",sz","sph",0,
      "tx","ty","tz",0
    };

}

#endif
