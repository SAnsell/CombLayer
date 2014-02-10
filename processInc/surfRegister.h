/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/surfRegister.h
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
#ifndef modelSupport_surfRegister_h
#define modelSupport_surfRegister_h


namespace Geometry
{
  class Surface;
  class Plane;
  class Vec3D;
}

namespace ModelSupport
{

/*!
  \class surfRegister 
  \version 1.0
  \author S. Ansell
  \date November 2009
  \brief Control surface numbers
*/

class surfRegister
{
 private:
  
  typedef std::map<int,int> MTYPE;       ///< Storage type
  MTYPE Index;                           ///< Index of kept surfaces

  void addToIndex(const int,const int);

 public:
  
  surfRegister();
  surfRegister(const surfRegister&);
  surfRegister& operator=(const surfRegister&);
  ~surfRegister() {} ///< Destructor

  void reset();      
  void addMatch(const int,const int);
  int registerSurf(const int,Geometry::Surface*);
  int registerSurf(Geometry::Surface*);

  bool hasSurf(const int) const;
  Geometry::Plane* createMirrorPlane(const Geometry::Surface*) const;
  int realSurf(const int) const;
  void process(std::vector<int>&) const;
  Geometry::Surface* realSurfPtr(const int) const;
  template<typename T> T* realPtr(const int) const;
  template<typename T> T* findPtr(const Geometry::Vec3D&,
	    const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  void setKeep(const int,const int) const;
};

}

#endif
