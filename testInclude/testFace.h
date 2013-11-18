/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testFace.h
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
#ifndef testFace_h
#define testFace_h 

/*!
  \struct faceObj
  \version 1.0
  \author S. Ansell
  \date March 2011
  \brief Create and hold a face object
*/

struct faceObj
{
  Geometry::Vertex* VPtr[3];           ///< Vertex Ptr
  Geometry::Edge* EPtr[3];             ///< Edges of a Face object
  Geometry::Face* FPtr;                ///< Face pointer

  faceObj(const int,const Geometry::Vec3D&,
	  const Geometry::Vec3D&,const Geometry::Vec3D&);
  faceObj(const faceObj&);
  faceObj& operator=(const faceObj&);
  ~faceObj();

  /// Access operator
  const Geometry::Face* operator->() const { return FPtr; }
  /// Access operator
  const Geometry::Face& operator*() const { return *FPtr; }
  /// Access operator to ptr
  const Geometry::Face* getPtr() const { return FPtr;}  
};

/*!
  \class testFace
  \brief Face : Face intersection
  \author S. Ansell
  \date February 2011
  \version 1.0

  Test the intersection of faces
*/

class testFace 
{
private:

  //Tests 
  int testInTriangle();
  int testNonPlanar();
  int testPlanar();

public:

  testFace();
  ~testFace();

  int applyTest(const int);     
};

#endif
