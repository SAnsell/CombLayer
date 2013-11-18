/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/Track.h
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
#ifndef MonteCarloTrack_h
#define MonteCarloTrack_h

namespace MonteCarlo
{

class Object;

/*!
  \class TUnits
  \version 1.0
  \author S. Ansell
  \brief Linked point of each track point
  \date December 2009
 */

struct TUnits
{
  Geometry::Vec3D InitPt;        ///< initial point 
  Geometry::Vec3D ExitPt;        ///< Final point
  double length;                 ///< track unit length
  const Object* OCPtr;      ///< Object Found
  
  TUnits(const Geometry::Vec3D&,const Geometry::Vec3D&,const Object*);
  TUnits(const TUnits&);
  TUnits& operator=(const TUnits&);
  ~TUnits() {}   ///< Destructor

  void write(std::ostream&) const;
};

/*!
  \class Track
  \version 1.0
  \author S. Ansell
  \brief Order List of track units.
  \date January 2008
  
  Holds track through real material.
 */

class Track
{
 private:

  Geometry::Vec3D iPt;              ///< Start Point
  const Object* iObj;         ///< Initial object

  std::vector<TUnits> Items;        ///< Items found

 public:

  Track();
  Track(const Geometry::Vec3D&);
  Track(const Geometry::Vec3D&,const Object*);
  Track(const Track&);
  Track& operator=(const Track&);
  ~Track();
  
  void addPoint(const Object*,const Geometry::Vec3D&);
  void setFirst(const Geometry::Vec3D&);
  /// access Track units
  std::vector<TUnits>& getTLine() { return Items; }

};

std::ostream&
operator<<(std::ostream&,const TUnits&);

} // NAMESPACE MonteCarlo

#endif
