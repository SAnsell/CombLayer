/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/SecondTrack.h
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
#ifndef SecondTrack_h
#define SecondTrack_h

namespace attachSystem
{
/*!
  \class SecondTrack
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Component that is at a fixed position
*/

class SecondTrack  
{
 protected:
  
  Geometry::Vec3D bX;            ///< X-coordinate 
  Geometry::Vec3D bY;            ///< Y-coordinate 
  Geometry::Vec3D bZ;            ///< Z-coordinate 

  Geometry::Vec3D bEnter;        ///< Beam at enterance
  Geometry::Vec3D bExit;         ///< Beam Exit
  LinkUnit LExit;                ///< Exit link unit

  void createUnitVector(const FixedComp&);
  void createUnitVector(const SecondTrack&);

 public:

  SecondTrack();
  SecondTrack(const SecondTrack&);
  SecondTrack& operator=(const SecondTrack&);
  virtual ~SecondTrack() {}     ///< Destructor
  
  /// Access X
  const Geometry::Vec3D& getBX() const 
    { return bX; }  
  /// Access Y direction
  const Geometry::Vec3D& getBY() const 
    { return bY; }  
  /// Access Z direction
  const Geometry::Vec3D& getBZ() const 
    { return bZ; }
  /// Access Start poitn 
  virtual const Geometry::Vec3D& getBeamStart() const 
    { return bEnter; }  
  /// Return BeamTrack [exit]
  virtual const Geometry::Vec3D& getBeamAxis() const 
    { return bY; }  
  /// Return Exit
  virtual const Geometry::Vec3D& getBeamExit() const 
    { return bExit; }  

  void setBeamExit(const Geometry::Vec3D&,
		   const Geometry::Vec3D&);
  int getExitSurf() const;
  const Geometry::Vec3D& getExitAxis() const;
  std::string getExitString() const;

  virtual void applyRotation(const Geometry::Vec3D&,const double);
  virtual void applyBeamAngleRotate(const double,const double);
};

}

#endif
 
