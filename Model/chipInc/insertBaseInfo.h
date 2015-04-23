/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/insertBaseInfo.h
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
#ifndef insertBaseInfo_h
#define insertBaseInfo_h

class Simulation;

namespace ModelSupport
{
  class surfRegister;
}

namespace shutterSystem
{

/*!
  \class insertBaseInfo
  \version 1.0
  \author S. Ansell
  \date May 2009
  \brief Hold system for building inserts

*/						
class insertBaseInfo
{
 private:

  ModelSupport::surfRegister& SMapRef; ///< SurfMap Pointer (if in use)

  Simulation& System;          ///< System to get info from 
  Geometry::Vec3D PipeAxis;    ///< PipeAxis
  Geometry::Vec3D PipeCent;    ///< PipeCentre

  Geometry::Vec3D Axis;        ///< Axis of insert unit
  Geometry::Vec3D Z;           ///< Z-Axis of insert unit
  Geometry::Vec3D Pt;          ///< Point of start
  double ZOffset;              ///< ZShift of the shutter

  double fullLength;           ///< Full length of sections
  int fPlane;                  ///< Back Plane number
  int bPlane;                  ///< Front Plane number

  std::vector<int> Sides;      ///< Sides [square]
  
  std::vector<Geometry::Cylinder*> Radius;       ///< Radii
  std::vector<Geometry::Plane*> Boundary;        ///< Boundary
  std::vector<insertInfo> Components;            ///< Components to build

  std::string cylinderIntercept(const int,const int,const int) const; 
  int continuityCheck();
  void createSides();
  std::string fullSides() const;
  
 public:

  insertBaseInfo(Simulation&,ModelSupport::surfRegister&);
  insertBaseInfo(const insertBaseInfo&);
  insertBaseInfo& operator=(const insertBaseInfo&);
  ~insertBaseInfo() {}  ///< Destructor

  void setInit(const Geometry::Vec3D&,const Geometry::Vec3D&,
	       const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setFPlane(const int F) { fPlane=F; }    ///< set the forward plane
  void populate(const std::string&); 
  void createObjects(const int,int&); 
  void setSides(const int,const int,const int,const int);
  std::vector<Geometry::Vec3D> calcSidesIntercept(const int,const int) const;
  
  /// Access plane
  int getBPlane() const { return bPlane; }               
  /// Access fullLength
  double getFullLength() const { return fullLength; }    
  /// Access Point
  Geometry::Vec3D getFirstPt() const { return Pt; }
  /// Last Access Point
  Geometry::Vec3D getLastPt() const { return Pt+Axis*fullLength; }
  /// Access Axis
  const Geometry::Vec3D& getPrimaryAxis() const { return Axis; }
  /// Access Pipe Centre
  const Geometry::Vec3D& getPipePt() const { return PipeCent; }
  /// Access Pipe Axis
  const Geometry::Vec3D& getPipeAxis() const { return PipeAxis; }

  Geometry::Vec3D calcHorrVec() const;

  std::string getFullSides() const;
  void printComponents() const;

};

}

#endif
