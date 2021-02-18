/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/LineIntersectVisit.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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

class LineIntersectVisit : public Global::BaseVisit
  {
  private:

    Geometry::Line ATrack;                             ///< Line 
    std::vector<Geometry::Vec3D> PtVec;                ///< Output point 
    std::vector<double> distVec;                       ///< Output distances
    std::vector<const Geometry::Surface*> surfVec;     ///< SurfNames
    long int neutIndex;                                ///< Particle number

    void procTrack(const Geometry::Surface*);
    ///\cond PRIVATE
    LineIntersectVisit(const LineIntersectVisit&);
    LineIntersectVisit& operator=(const LineIntersectVisit&);
    ///\endcond PRIVATE

  public:
    
    LineIntersectVisit(const Geometry::Vec3D&,const Geometry::Vec3D&);
    LineIntersectVisit(const Geometry::Line&);
    LineIntersectVisit(const MonteCarlo::particle&);

    /// Destructor
    virtual ~LineIntersectVisit() {};

    void Accept(const Geometry::Surface&);
    void Accept(const Geometry::Quadratic&);
    void Accept(const Geometry::ArbPoly&);
    void Accept(const Geometry::Cone&);
    void Accept(const Geometry::CylCan&);
    void Accept(const Geometry::Cylinder&);
    void Accept(const Geometry::EllipticCyl&);
    void Accept(const Geometry::General&);
    void Accept(const Geometry::MBrect&);
    void Accept(const Geometry::Plane&);
    void Accept(const Geometry::Sphere&);
    void Accept(const Geometry::Torus&);

    void Accept(const HeadRule&);

    /// Clear track
    void clearTrack() 
      { PtVec.clear(); distVec.clear(); surfVec.clear(); } 

    /// Distance Accessor
    const std::vector<double>& getDistance() const 
      { return distVec; }

    /// Point Accessor
    const std::vector<Geometry::Vec3D>& getPoints() const { return PtVec; }
    /// Index Accessor
    const std::vector<const Geometry::Surface*>& getSurfIndex() const 
      { return surfVec; }

    const std::vector<Geometry::Vec3D>& getPoints(const Geometry::Surface*);
    const std::vector<Geometry::Vec3D>& getPoints(const HeadRule&);
    const std::vector<Geometry::Vec3D>& getPoints(const Geometry::Surface*,
		  const Geometry::Surface*,const int);

    Geometry::Vec3D getPoint(const Geometry::Surface*);
    Geometry::Vec3D getPoint(const Geometry::Surface*,
			     const Geometry::Surface*,const int);
    Geometry::Vec3D getPoint(const Geometry::Surface*,
			     const Geometry::Vec3D&);
    Geometry::Vec3D getPoint(const std::string&,
			     const Geometry::Vec3D&);
    Geometry::Vec3D getPoint(HeadRule&,
			     const Geometry::Vec3D&);
    /// Get number in intersection
    size_t getNPoints() const { return PtVec.size(); }

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
