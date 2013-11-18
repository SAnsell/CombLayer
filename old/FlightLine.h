/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/FlightLine.h
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
#ifndef FlightLine_h
#define FlightLine_h

class Simulation;

namespace LensSystem
{

/*!
  \class FlightLine
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Flight line storage
*/

class FlightLine 
{
 protected:

  Geometry::Vec3D Axis;   ///< Flight axis
  Geometry::Vec3D Z;      ///< Flight Z axis
  Geometry::Vec3D X;      ///< Flight X axis

  Geometry::Vec3D Centre; ///< Centre Point

  double FDist;           ///< Start distance [From centre]
  double EDist;           ///< End distance [-ve infinity]

  double Width;           ///< Flight axis width
  double Height;          ///< Flight axis width

  int StopSurf;           ///< Stopping surface [edge]
  std::vector<Geometry::Plane*> Sides; ///< Surfaces of sides [inlooking]

  std::pair<int,int> zMatch(const FlightLine&) const;
  int hasInterceptLeft(const FlightLine&,std::pair<int,int>&) const;
  int hasInterceptRight(const FlightLine&,std::pair<int,int>&) const;

 public:

  FlightLine();
  FlightLine(const Geometry::Vec3D&,const Geometry::Vec3D&,
	     const double,const double,const double);
  FlightLine(const FlightLine&);
  FlightLine& operator=(const FlightLine&);
  ~FlightLine() {}  ///< Destructor

  int getLeft() const { return (!Sides.empty() ? Sides[0]->getName() : 0); }
  int getRight() const { return (!Sides.empty() ? Sides[1]->getName() : 0); }
  int getFloor() const { return (!Sides.empty() ? Sides[2]->getName() : 0); }
  int getRoof() const { return (!Sides.empty() ? Sides[3]->getName() : 0); }
  
  /// Access Stop Surface:
  void addStopSurf(const int N) { StopSurf=N; }
  void generateSurfaces(const int);
  std::string createCell(const FlightLine*,const FlightLine*) const;

};

}


#endif
 
