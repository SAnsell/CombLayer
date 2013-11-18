/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/ObjectTrackAct.h
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
#ifndef ModelSupport_ObjectTrackAct_h
#define ModelSupport_ObjectTrackAct_h

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
  \class ObjectTrackAct
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

class ObjectTrackAct
{
 private:

  /// Target point
  Geometry::Vec3D TargetPt;  
  /// Main data information set [Object : ItemTrack]
  std::map<int,LineTrack> Items; 
  
 public:

  ObjectTrackAct(const Geometry::Vec3D&);
  ObjectTrackAct(const ObjectTrackAct&);
  ObjectTrackAct& operator=(const ObjectTrackAct&);  
  /// Destructor
  ~ObjectTrackAct() {}

  void clearAll();
  //  void addDistance(const int,const int,const double);
  /// Set target point
  void setTarget(const Geometry::Vec3D& Pt) { TargetPt=Pt; }

  void addUnit(const Simulation&,const int,Geometry::Vec3D);

  double getMatSum(const int) const;
  /// Debug function effectivley
  //  const std::map<int,ObjTrackItem>& getMap() const { return Items; }

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const ObjectTrackAct&);

}

#endif
