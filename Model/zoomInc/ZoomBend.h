/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomBend.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef zoomSystem_ZoomBend_h
#define zoomSystem_ZoomBend_h

class Simulation;

namespace shutterSystem
{
  class GeneralShutter;
  class ZoomShutter;
  class BulkShield;
  class BulkInsert;
}

namespace zoomSystem
{
  /*!
    \class ZoomBend
    \version 1.0
    \author S. Ansell
    \date January 2010
    \brief Adds the Zoom bend
  */

class ZoomBend : public attachSystem::ContainedGroup,
    public attachSystem::TwinComp
{
 private:
  
  const int bendIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< populated or not

  Geometry::Vec3D BCentre;      ///< Rotation centre [centerline]
  Geometry::Vec3D normalOut;    ///< Normal out [To correct angle]
  Geometry::Vec3D zOut;         ///< Z out [to correct angle]
  Geometry::Vec3D BNormal;      ///< Normal direction to bend
  Geometry::Vec3D BCross;       ///< Cross direction

  double bendAngle;             ///< Angle of the beam [mRad]
  double bendVertAngle;         ///< Angle of the vertical direction
  double bendXAngle;            ///< Rotation of beam relative to entry [mRad]
  double bendRadius;            ///< Radius of bend
  double bendLength;            ///< Full length

  size_t NVanes;                   ///< Number of vanes  
  double bendWidth;             ///< neutron width
  double bendHeight;            ///< neutron height
  double vaneThick;             ///< Thickness of vane
  int vaneMat;                  ///< Vane material

  double xStep;                 ///< Offset on X Shutter exit point
  double yStep;                 ///< Offset on Y Shutter exit point
  double zStep;                 ///< Offset on Z Shutter exit point

  double bxStep;                ///< Bend offset relative to shutter point
  double bzStep;                ///< Bend offset relative to shutter point
  /// sections:
  std::array<bendSection,4> BSector;  
  
  size_t nAttn;                   ///< Number of attenuation points
  double attnZStep;               ///< Attenuation Z step
  std::vector<double> upperYPos;  ///< Y Pois
  std::vector<double> upperDist;  ///< Y Pois

  
  double wallThick;             ///< Wall thickness

  int innerMat;            ///< Inner shield material
  int wallMat;             ///< Wall material

  std::vector<int> innerShield;  ///< Inner shield sections
  int innerCell;           ///< Section for the guide


  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

  void createVanes(Simulation&); 
  void createAttenuation(Simulation&); 

 public:

  ZoomBend(const std::string&);
  ZoomBend(const ZoomBend&);
  ZoomBend& operator=(const ZoomBend&);
  virtual ~ZoomBend();
  
  /// Accessor to NormOut
  virtual const Geometry::Vec3D& getExitNorm() const 
    { return normalOut; }

  /// Access beam angle
  double getBendAngle() const { return bendAngle; }
  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;
  
  int getSectionSurf(const int,const int) const;
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
