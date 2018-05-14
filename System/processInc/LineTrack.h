/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/LineTrack.h
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
#ifndef ModelSupport_LineTrack_h
#define ModelSupport_LineTrack_h

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
  \class LineTrack
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief Track from A to B 
*/

class LineTrack
{
 private:
  
  const Geometry::Vec3D InitPt;     ///< Initial point
  const Geometry::Vec3D EndPt;      ///< Target point
  const double aimDist;             ///< Aim distance

  double TDist;                     ///< Total distance
  
  std::vector<long int> Cells;                    ///< Cells in order
  std::vector<MonteCarlo::Object*> ObjVec;        ///< Object pointers
  std::vector<const Geometry::Surface*> SurfVec;  ///< Surface pointers

  ///< Signed index [particle origin side true]
  std::vector<int> SurfIndex;                
  std::vector<double> Track;                ///< Track length

  bool updateDistance(MonteCarlo::Object*,
		      const Geometry::Surface*,
		      const int,const double);

 public:

  LineTrack(const Geometry::Vec3D&,const Geometry::Vec3D&);
  LineTrack(const Geometry::Vec3D&,const Geometry::Vec3D&,const double);
  LineTrack(const LineTrack&);
  LineTrack& operator=(const LineTrack&);    
  ~LineTrack() {}          ///< Destructor

  void clearAll();
  /// Determine if track is complete 
  bool isCompelete() const { return (aimDist-TDist) < -Geometry::zeroTol; }

  void calculate(const Simulation&);
  void calculateError(const Simulation&);
  /// Access Cells
  const std::vector<long int>& getCells() const
    { return Cells; }
  /// Access Track lengths
  const std::vector<double>& getTrack() const
    { return Track; }
  /// Access Object Pointers
  const std::vector<MonteCarlo::Object*>& getObjVec() const
  { return ObjVec; }
  /// Access Surface Pointers
  const std::vector<const Geometry::Surface*>& getSurfVec() const
    { return SurfVec; }
  /// Access Surface Pointers
  const std::vector<int>& getSurfIndex() const
    { return SurfIndex; }

  Geometry::Vec3D getPoint(const size_t) const;
  int getSurfIndex(const size_t) const;
  const Geometry::Surface* getSurfPtr(const size_t) const;
  
  /// access total distance
  double getTotalDist() const { return aimDist; }

  void createAttenPath(std::vector<long int>&,std::vector<double>&) const;
  void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const LineTrack&);

}

#endif
