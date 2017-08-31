/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/ObjectTrackPoint.h
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
#ifndef ModelSupport_ObjectTrackPoint_h
#define ModelSupport_ObjectTrackPoint_h

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

  class LineTrack;

/*!
  \class ObjectTrackPoint
  \version 1.0
  \author S. Ansell
  \date November 2010
  \brief Object to Point 

  This takes a simulation after each vertex point has been calculated
  for the given object.
  
  -- Iterate over each object and track to the target point
  -- Create a ObjTrackItem for each object.
  -- Support calculations based on the ObjTrackItem.
*/

class ObjectTrackPoint : public ObjectTrackAct
{
  private:

  /// Target point
  Geometry::Vec3D TargetPt;
  
 public:

  ObjectTrackPoint(const Geometry::Vec3D&);
  ObjectTrackPoint(const ObjectTrackPoint&);
  ObjectTrackPoint& operator=(const ObjectTrackPoint&);  
  ~ObjectTrackPoint() {}   ///< Destructor

  /// Set target point
  void setTarget(const Geometry::Vec3D& Pt) { TargetPt=Pt; }

  void addUnit(const Simulation&,const long int,const Geometry::Vec3D&);

  /// Debug function effectivley
  //  const std::map<int,ObjTrackItem>& getMap() const { return Items; }

  void createAttenPath(std::vector<long int>&,std::vector<double>&) const;
  virtual void write(std::ostream&) const;

};

}

#endif
