/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModelInc/lensFL.h
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
#ifndef lensSystem_lensFL_h
#define lensSystem_lensFL_h

class Simulation;

namespace lensSystem
{

/*!
  \class lensFL
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Flight line storage
*/

class lensFL : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  Geometry::Vec3D Axis;   ///< Flight axis
  Geometry::Vec3D Z;      ///< Flight Z axis
  Geometry::Vec3D X;      ///< Flight X axis

  Geometry::Vec3D Centre; ///< Centre Point

  double Width;           ///< Flight axis width
  double Height;          ///< Flight axis width
  double liner;           ///< Liner thickness


  std::vector<Geometry::Plane*> Sides; ///< Surfaces of sides [inlooking]

 public:

  lensFL();
  lensFL(const Geometry::Vec3D&,const Geometry::Vec3D&,
	     const double,const double,const double D);
  lensFL(const lensFL&);
  lensFL& operator=(const lensFL&);
  ~lensFL() {}  ///< Destructor

  /// set liner
  void setLiner(const double D) { liner=D; }

  int hasInterceptLeft(const lensFL&,const Geometry::Surface*,
		       const int,std::pair<int,int>&) const;
  int hasInterceptRight(const lensFL&,const Geometry::Surface*,
		       const int,std::pair<int,int>&) const;

  /// Access Stop Surface:
  void generateSurfaces(ModelSupport::surfRegister&,const int);
  /// Get Axis vector
  const Geometry::Vec3D& getAxis() const { return Axis; }
  
  std::string getLine() const;
  std::string getVirtual() const;
  
};

}


#endif
 
