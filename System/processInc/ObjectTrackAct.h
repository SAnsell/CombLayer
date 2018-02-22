/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/ObjectTrackAct.h
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
  \brief Object to Point / Plane etc

  This takes a simulation after each vertex point has been calculated
  for the given object.
  
  -- Iterate over each object and track to the target point
  -- Create a ObjTrackItem for each object.
  -- Support calculations based on the ObjTrackItem.
*/


class ObjectTrackAct
{
 protected:

  /// Storage type for line units
  typedef std::map<long int,LineTrack> itemTYPE;
  /// Main data information set [Object : ItemTrack]
  itemTYPE Items; 
  
 public:

  ObjectTrackAct();
  ObjectTrackAct(const ObjectTrackAct&);
  ObjectTrackAct& operator=(const ObjectTrackAct&);  
  /// Destructor
  virtual ~ObjectTrackAct() {}

  void clearAll();

  double getMatSum(const long int) const;
  double getAttnSum(const long int) const;
  double getAttnSum(const long int,const double) const;
  double getDistance(const long int) const;
  /// Debug function effectivley
  //  const std::map<int,ObjTrackItem>& getMap() const { return Items; }
  void createAttenPath(std::vector<long int>&,std::vector<double>&) const;
  virtual void write(std::ostream&) const =0;

};

std::ostream&
operator<<(std::ostream&,const ObjectTrackAct&);

}

#endif
