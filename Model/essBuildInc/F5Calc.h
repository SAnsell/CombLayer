/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/F5Calc.h
 *
 * Copyright (c) 2015-2017 Konstantin Batkov
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
#ifndef tallySystem_F5Calc
#define tallySystem_F5Calc

namespace tallySystem
{
/*!
  \class F5Calc
  \version 1.0
  \author Konstantin Batkov 
  \date May 2015
  \brief F5 collimator system projection calculation system
  \todo This is not a good way to do this -- 
*/

class F5Calc
{
 private:
  
  Geometry::Vec3D B;   ///< corner corners
  Geometry::Vec3D C;   ///< Mid point
  Geometry::Vec3D B2;   ///< Lower corner corners

  Geometry::Vec3D F5; ///< F5 tally coordinates

  /// Distance of the middle of the collimator entrance from the point detector
  double af;

  ///Coordinates of point A, the start of the collimator on the M - F5 line
  Geometry::Vec3D A; 

  /// Middle of the moderator
  Geometry::Vec3D M; 
    
  Geometry::Vec3D CalculateNormalVector(const Geometry::Vec3D&,
					const Geometry::Vec3D&,
					const Geometry::Vec3D&) const;
    
  Geometry::Vec3D CalculateProjectionOntoPlane(const Geometry::Vec3D&,
					       const Geometry::Vec3D&,
					       const Geometry::Vec3D&) const;
  
 public:

  F5Calc();
  F5Calc(const F5Calc&);
  F5Calc& operator=(const F5Calc&);
  ~F5Calc();
  
  void SetTally(double,double,double); 
  void SetPoints(const Geometry::Vec3D&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&); 
  void SetLength(double); 
  
  void CalculateMiddleOfModerator();
  double GetZAngle();
  double GetXYAngle();
  double GetHalfSizeX() const; 
  double GetHalfSizeZ() const; 

  /// returns the collimator solid angle
  double GetOmega() const
    { return GetHalfSizeX()*GetHalfSizeZ()*4.0/af/af; } 
  
 };
 
}

#endif
