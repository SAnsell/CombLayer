/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/TypeString.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef ColErr_TypeString_h
#define ColErr_TypeString_h

/*!
  \namespace ColErr
  \brief Get a type string for exception types
  \author S. Ansell
  \version 1.0
  \date November 2018
  
*/

namespace Geometry
{
  class Vec3D;
  class Edge;
  class Face;
  class Surface;
}

namespace ModelSupport
{
  class ObjSurfMap;
}

namespace MonteCarlo
{
  class Material;
  class Object;
}
class objectGroups;

namespace ColErr
{

  template<typename T>
     const std::string typeIDName();
  
  template<>
    const std::string typeIDName<char>()
    { return "char"; } 
  template<>
    const std::string typeIDName<std::string>()
    { return "std::string"; } 
  template<>
    const std::string typeIDName<unsigned int>()
    { return "unsigned int"; } 
  template<>
    const std::string typeIDName<int>()
    { return "int"; } 
  template<>
    const std::string typeIDName<size_t>()
    { return "size_t"; } 
  template<>
    const std::string typeIDName<long int>()
    { return "long int"; } 
  template<>
    const std::string typeIDName<double>()
    { return "double"; } 
  template<>
    const std::string typeIDName<void>()
    { return "void*"; } 
  template<>
    const std::string typeIDName<Geometry::Vec3D>()
    { return "Geometry::Vec3D"; } 

  // THESE CAN BE REMOVED WITH SOME template remove_ptr const_expr
  template<>
    const std::string typeIDName<std::string*>()
    { return "std::string*"; } 
  template<>
    const std::string typeIDName<int*>()
    { return "int*"; } 
  template<>
    const std::string typeIDName<unsigned int*>()
    { return "unsigned int*"; } 
  template<>
    const std::string typeIDName<long*>()
    { return "long*"; } 
  template<>
    const std::string typeIDName<double*>()
    { return "double*"; } 
  template<>
    const std::string typeIDName<unsigned long*>()
    { return "unsigned long*"; } 
  template<>
    const std::string typeIDName<Geometry::Face*>()
    { return "Face*"; } 
  template<>
    const std::string typeIDName<Geometry::Edge*>()
    { return "Edge*"; } 
  template<>
    const std::string typeIDName<Geometry::Surface*>()
    { return "Geometry::Surface*"; } 
  template<>
    const std::string typeIDName<Geometry::Vec3D*>()
    { return "Geometry::Vec3D*"; } 
  template<>
    const std::string typeIDName<MonteCarlo::Object*>()
    { return "MonteCarlo::Object*"; } 
  template<>
    const std::string typeIDName<MonteCarlo::Material*>()
    { return "MonteCarlo::Material*"; } 
  template<>
    const std::string typeIDName<ModelSupport::ObjSurfMap*>()
    { return "ModelSupport::ObjSurfMap*"; } 
  template<>
    const std::string typeIDName<objectGroups*>()
    { return "objectGroups*"; } 

  
}  // NAMESPACE  ColErr

#endif
