/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/ObjSurfMap.h
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
#ifndef ModelSupport_ObjSurfMap_h
#define ModelSupport_ObjSurfMap_h

namespace Geometry
{
  class Surface;
}

namespace MonteCarlo
{
  class Object;
}

namespace ModelSupport
{

/*!
  \class ObjSurfMap
  \version 1.0
  \author S. Ansell
  \date November 2010
  \brief Surface number to Object map
*/

class ObjSurfMap
{

 public:

  /// Object set
  typedef std::vector<MonteCarlo::Object*> STYPE;
  typedef std::set<int> surfTYPE;          ///< surfaces used
  typedef std::map<int,STYPE> OMTYPE;      ///< +/-SurfN : ObjecPtr
  typedef std::map<int,surfTYPE> OSTYPE;   ///< objName : surfSet
   
 private:

  OMTYPE SMap;                    ///< SurfNumber : Object map
  OSTYPE OSurfMap;                ///< ObjectName : SurfNumbers
  
  void addSurface(const int,MonteCarlo::Object*);
  void addObjectSurf(const MonteCarlo::Object*,const int);
  void removeObjectSurface(const int);
  
 public:

  ObjSurfMap();
  ObjSurfMap(const ObjSurfMap&);
  ObjSurfMap& operator=(const ObjSurfMap&);
  ~ObjSurfMap() {}          ///< Destructor

  void clearAll();
  
  void addSurfaces(MonteCarlo::Object*);
  
  MonteCarlo::Object* getObj(const int,const size_t) const;
  const STYPE& getObjects(const int) const;
  MonteCarlo::Object* findNextObject(const int,
				     const Geometry::Vec3D&,const int) const;

  const std::set<int>& connectedObjects(const int) const;
  
  void removeReverseSurf(const int,const int);

  void write(const std::string&) const;
  void write(std::ostream&) const;

  static void removeEqualSurf(const std::map<int,Geometry::Surface*>&,
			      std::map<int,MonteCarlo::Qhull*>&);
};



}

#endif
