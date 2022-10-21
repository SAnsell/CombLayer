/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/BenderUnit.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef beamlineSystem_BenderUnit_h
#define beamlineSystem_BenderUnit_h


namespace beamlineSystem
{

/*!
  \class BenderUnit
  \version 1.0
  \author S. Ansell
  \date September 2014
  \brief Points associated with tracked beamline bender
*/

class BenderUnit :
    public GuideUnit
{
 private:
  
  double aHeight;             ///< Height across rotation plane [start]
  double bHeight;             ///< Across rotation plane [end]
  double aWidth;              ///< In rotation plane [start]
  double bWidth;              ///< In rotation plane [end]

  double radius;              ///< Primary rotation ratius
  double rotAng;              ///< Rotation angle (radian)

  Geometry::Vec3D RCent;      ///< Rotation centre
  
  Geometry::Vec3D calcWidthCent(const bool) const;
  virtual void createSurfaces();
  virtual void createObjects(Simulation&);
  virtual void createLinks();
  
 public:

  BenderUnit(const std::string&);
  BenderUnit(const BenderUnit&);
  BenderUnit& operator=(const BenderUnit&);
  virtual BenderUnit* clone() const;
  virtual ~BenderUnit();

  void setValues(const double,const double,const double,
		 const double,const double);
  void setValues(const double,const double,const double,
		 const double,const double,const double,
		 const double);
  void setOriginAxis(const Geometry::Vec3D&,const Geometry::Vec3D&,
		     const Geometry::Vec3D&,const Geometry::Vec3D&);

  // Accessor to beg axis
  Geometry::Vec3D getBegAxis() const { return -AYVec; }
  // Accessor to end axis
  Geometry::Vec3D getEndAxis() const { return BYVec; }

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const FixedComp&,const long int);

};

}

#endif
 
