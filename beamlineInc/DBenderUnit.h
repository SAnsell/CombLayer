/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/DBenderUnit.h
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
#ifndef beamlineSystem_DBenderUnit_h
#define beamlineSystem_DBenderUnit_h


namespace beamlineSystem
{

/*!
  \class DBenderUnit
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Points associated with tracked double beamline bender
*/

class DBenderUnit : public GuideUnit
{
 private:

  Geometry::Vec3D RCentA;      ///< Rotation centre
  Geometry::Vec3D RCentB;      ///< Rotation centre
  Geometry::Vec3D RAxisA;      ///< Rotation axis [horr plane ]
  Geometry::Vec3D RAxisB;      ///< Rotation axis [vert plane]
  
  Geometry::Vec3D RPlaneFrontA;    ///< Towards Centre direction
  Geometry::Vec3D RPlaneFrontB;    ///< Towards Centre direction
  Geometry::Vec3D RPlaneBackA;     ///< Towards Centre direction
  Geometry::Vec3D RPlaneBackB;     ///< Towards Centre direction

  Geometry::Vec3D endPtA;          ///< A plane endpoint 
  Geometry::Vec3D endPtB;          ///< B plane end point
  
  double RadiusA;             ///< Primary rotation ratius
  double RadiusB;             ///< Primary rotation ratius
  double aHeight;             ///< Height across rotation plane [start]
  double bHeight;             ///< Across rotation plane [end]
  double aWidth;              ///< In rotation plane [start]
  double bWidth;              ///< In rotation plane [end]
  double Length;              ///< Length of section
  double rotAng;              ///< Rotation of Z bend axis for bend
  double sndAng;              ///< Rotation of X bend axis for bend

  Geometry::Vec3D AXVec;    ///< Current XVector [Front]
  Geometry::Vec3D AYVec;    ///< Current YVector [Front]
  Geometry::Vec3D AZVec;    ///< Current ZVector [Front]

  Geometry::Vec3D BXVec;    ///< Current XVector [Back] 
  Geometry::Vec3D BYVec;    ///< Current YVector [Back]
  Geometry::Vec3D BZVec;    ///< Current ZVector [Back]

  Geometry::Vec3D calcWidthCent(const bool) const;
  Geometry::Vec3D calcHeightCent(const bool) const;

  void createSurfaces() override;
  void createObjects(Simulation&) override;
  void createLinks() override;
  
 public:

  DBenderUnit(const std::string&);
  DBenderUnit(const DBenderUnit&);
  DBenderUnit& operator=(const DBenderUnit&);
  virtual DBenderUnit* clone() const;
  ~DBenderUnit() override;

  void setApperture(const double,const double,
		    const double,const double);
  void setRadii(const double,const double);
    
  void setLength(const double);
  // Set both rotation angles
  void setRotAngle(const double A,const double B)
  { rotAng=A; sndAng=B; }
  
  void setOriginAxis(const Geometry::Vec3D&,const Geometry::Vec3D&,
		     const Geometry::Vec3D&,const Geometry::Vec3D&);


  using FixedComp::createAll;
  void createAll(Simulation&,const FixedComp&,const long int) override;

};

}

#endif
 
