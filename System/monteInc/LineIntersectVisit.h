/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/LineIntersectVisit.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef LineIntersectVisit_h
#define LineIntersectVisit_h

class HeadRule;

namespace Geometry
{
  class interPoint;
}

namespace MonteCarlo
{
  class particle;

/*!
  \class LineIntersectVisit
  \author S. Ansell
  \version 1.0
  \date September 2007 
  \brief Intersect of Line with a surface 

  Creates interaction with a line
*/

class LineIntersectVisit :
    public Global::BaseVisit
{
private:
  
  Geometry::Line ATrack;                             ///< Line 
  std::vector<Geometry::interPoint> IPts;           /// 

  // std::vector<Geometry::Vec3D> PtVec;                ///< Output point 
  // std::vector<double> distVec;                       ///< Output distances
  // std::vector<int> surfNumber;                       ///< SurfIndexes [signed]
  // std::vector<const Geometry::Surface*> surfVec;     ///< Surfaces
  long int neutIndex;                                ///< Particle number
  
  void procTrack(const std::vector<Geometry::Vec3D>&,
		 const Geometry::Surface*);

  //  Geometry::interPoint& getClosest();
  const Geometry::interPoint& getClosest() const;
  
  ///\cond PRIVATE
  LineIntersectVisit(const LineIntersectVisit&);
  LineIntersectVisit& operator=(const LineIntersectVisit&);
  ///\endcond PRIVATE
  
public:
  
  LineIntersectVisit(const Geometry::Vec3D&,const Geometry::Vec3D&);
  LineIntersectVisit(const Geometry::Line&);
  LineIntersectVisit(const MonteCarlo::particle&);
  
  /// Destructor
  ~LineIntersectVisit() override {};
  
  void Accept(const Geometry::Surface&) override;
  void Accept(const Geometry::Quadratic&) override;
  void Accept(const Geometry::ArbPoly&) override;
  void Accept(const Geometry::Cone&) override;
  void Accept(const Geometry::CylCan&) override;
  void Accept(const Geometry::Cylinder&) override;
  void Accept(const Geometry::EllipticCyl&) override;
  void Accept(const Geometry::General&) override;
  void Accept(const Geometry::MBrect&) override;
  void Accept(const Geometry::Plane&) override;
  void Accept(const Geometry::Sphere&) override;
  void Accept(const Geometry::Torus&) override;
  
  void Accept(const HeadRule&) override;
  

  void clearTrack();
  const std::vector<Geometry::interPoint>& getInterVec() const 
  { return IPts; }
  
  /// Distance Accessor
  // const std::vector<double>& getDistance() const 
  // { return distVec; }

  /*
  /// Point Accessor
  const std::vector<Geometry::Vec3D>& getPoints() const { return PtVec; }
  /// Pointer Accessor
  const std::vector<int>& getSurfIndexX() const 
  { return surfNumber; }
  /// Index Accessor
  const std::vector<const Geometry::Surface*>& getSurfPointers() const 
  { return surfVec; }
  */
  
  const std::vector<Geometry::interPoint>& getIntercept(const Geometry::Surface*);
  const std::vector<Geometry::interPoint>& getIntercept(const HeadRule&);
  const std::vector<Geometry::interPoint>& getIntercept
  (const Geometry::Surface*,const Geometry::Surface*,
   const int);
  
  // const std::vector<Geometry::Vec3D>& getPoints(const Geometry::Surface*);
  // const std::vector<Geometry::Vec3D>& getPoints(const HeadRule&);
  // const std::vector<Geometry::Vec3D>& getPoints(const Geometry::Surface*,
  // 						const Geometry::Surface*,
  // 						const int);
  
  Geometry::Vec3D getPoint(const Geometry::Surface*);
  Geometry::Vec3D getPoint(const Geometry::Surface*,
			   const Geometry::Surface*,const int);
  Geometry::Vec3D getPoint(const Geometry::Surface*,
			   const Geometry::Vec3D&);
  Geometry::Vec3D getPoint(HeadRule&,
			   const Geometry::Vec3D&);
  /// Get number in intersection
  size_t getNPoints() const;
  
  // Re-set the line
  void setLine(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setLine(const particle&);
  
  // Accessors:
  double getDist(const Geometry::Surface*);
  double getForwardDist(const Geometry::Surface*);
  
  /// Access Line
  const Geometry::Line& getTrack() const { return ATrack; } 
  
  void write(std::ostream&) const;
};
  
std::ostream& operator<<(std::ostream&,const LineIntersectVisit&);

}  // NAMESPACE MonteCarlo

#endif
