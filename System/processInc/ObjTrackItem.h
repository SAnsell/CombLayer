/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/ObjTrackItem.h
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
#ifndef ModelSupport_ObjTrackItem_h
#define ModelSupport_ObjTrackItem_h

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
  \class ObjTrackItem
  \version 1.0
  \author S. Ansell
  \date November 2010
  \brief Track of a path from A->B 
*/

class ObjTrackItem
{
 private:
  
  const Geometry::Vec3D InitPt;     ///< Initial point
  const Geometry::Vec3D EndPt;      ///< Target point
  const double aimDist;             ///< Aim distance

  int objName;            ///< Initial ObjName
  double TDist;           ///< Total distance
  double impDist;         ///< Distance in importance zero
  
  std::map<int,double> MTrack;    ///< Material track [MatN : dist ]
  
  ///\cond FORBIDDEN
  ObjTrackItem& operator=(const ObjTrackItem&);  
  ///\endcond FORBIDDEN

 public:

  ObjTrackItem(const Geometry::Vec3D&,const Geometry::Vec3D&);
  ObjTrackItem(const ObjTrackItem&);
  
  /// Destructor
  ~ObjTrackItem() {}

  void clearAll();
  /// Determine if track is complete 
  bool isCompelete() const { return (aimDist-TDist)<1e-4; }

  void addDistance(const int,const double);
  /// Accessor to track
  const std::map<int,double>& getTrack() const { return MTrack; }
  
  double getMatSum() const;
  void write(std::ostream&) const;
};

std::ostream& operator<<(std::ostream&,const ObjTrackItem&);

}

#endif
