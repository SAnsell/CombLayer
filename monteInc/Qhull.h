/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/Qhull.h
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
#ifndef Qhull_h
#define Qhull_h

namespace Geometry
{
  class Surface;
}

namespace MonteCarlo
{
  class SurfVertex;

/*!
  \class Qhull
  \version 1.0
  \date August 2005
  \author S.Ansell
  \brief Holds the intersection and vertex for a cell

  Processes the hull. At the moment calculates intersect lines
  and vertex points
*/

class Qhull : public Object
{
 private:
  
  std::vector<SurfVertex> VList;      ///< Full Vertex list
  Geometry::Vec3D CofM;               ///< Effective centre of mass

  int getIntersect(const Geometry::Surface*,const Geometry::Surface*,
		   const Geometry::Surface*);

  void calcCentreOfMass();

 public:
  
  Qhull();
  Qhull(const int,const int,const double,const std::string&);
  Qhull(const Qhull&);
  Qhull& operator=(const Qhull&);
  virtual Qhull* clone() const;
  virtual ~Qhull();

  /// Determine if intersections has been calculted:
  bool hasIntersections() const { return !VList.empty(); }
  int calcIntersections();
  int calcVertex();
  int calcMidVertex();
  
  Geometry::Matrix<double> getRotation(const unsigned int,const unsigned int,
			     const unsigned int,const unsigned int) const;
  Geometry::Vec3D getDisplace(const unsigned int) const;
  /// Access centre of mass
  const Geometry::Vec3D& getCofM() const { return CofM; }

  int isConnected(const int) const;

  // Output
  void printVertex() const;
  void printVertex(const Geometry::Vec3D&,const Geometry::Matrix<double>&) const;
  //  std::vector<Geometry::Vec3D>& getVertexPoints();
  std::string cellStr(const std::map<int,Qhull*>&) const;
  std::vector<Geometry::Vec3D> getVertex() const;

  virtual void displace(const Geometry::Vec3D&);
  virtual void rotate(const Geometry::Matrix<double>&); 
  virtual void mirror(const Geometry::Plane&);
};

} 

#endif
