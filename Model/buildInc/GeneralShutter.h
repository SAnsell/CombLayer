/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/GeneralShutter.h
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
#ifndef shutterSystem_GeneralShutter_h
#define shutterSystem_GeneralShutter_h

class Simulation;

/*!
  \namespace shutterSystem
  \brief Shutter components in the system
  \author S. Ansell
  \version 1.0
  \date September 2009
*/

namespace shutterSystem
{

  /*!
    \class GeneralShutter
    \version 2.0
    \author S. Ansell
    \date October 2009
    \brief Holds a simple basic ISIS shutter
    
    Can be specialized to ChipIR shutter or other
    Improved to not require shutter system
*/

class GeneralShutter : public attachSystem::TwinComp,
  public attachSystem::ContainedComp
{
 protected:

  const size_t shutterNumber;         ///< number of the shutter
  const std::string baseName;         ///< Basic name
  const int surfIndex;                ///< Index of the surface
  int cellIndex;                      ///< Index of the cells

  int populated;                      ///< populated / not

  int divideSurf;                     ///< Divider Number
  Geometry::Plane* DPlane;            ///< Divided plane [if set]

  double voidXoffset;                 ///< Main void vessel offset 
  double innerRadius;                 ///< Inner radius
  double outerRadius;                 ///< Outer radius

  double totalHeight;                 ///< Total height 
  double totalDepth;                  ///< Total depth
  double totalWidth;                  ///< Total Width
  double upperSteel;                  ///< length of top material
  double lowerSteel;                  ///< length of base material
  
  double shutterHeight;               ///< Full shutter height
  double shutterDepth;                ///< Full shutter depth
  double voidZOffset;                 ///< Offset of centre shutter [fixed]
  double centZOffset;                 ///< Offset of central line

  double closedZShift;                ///< Distance of down movement of shutter
  double openZShift;                  ///< Open shift [from axis]

  double voidDivide;                  ///< 18cm->24cm divider [0 for not there]
  double voidHeight;                  ///< Full section
  double voidHeightInner;             ///< Void height [inner]
  double voidWidthInner;              ///< Void width [inner]
  double voidHeightOuter;             ///< Void height [inner]
  double voidWidthOuter;              ///< Void width [inner]

  double clearGap;                    ///< Clearance gap after 
  double clearBoxStep;                ///< Step of the boxes
  double clearBoxLen;                 ///< length of step
  std::vector<double> clearCent;      ///< Centres
  
  double xyAngle;                     ///< floor angle
  double zAngle;                      ///< Angle on the shutter
  int shutterMat;                     ///< Support material

  int closed;                         ///< Is shutter closed
  int reversed;                       ///< Is shutter reversed

  std::vector<shutterBlock> SBlock; ///< Data for shutter inserts

  Geometry::Vec3D XYAxis;       ///< Beam axis [NOT rotated by zAngle]
  Geometry::Vec3D BeamAxis;     ///< Beam axis
  Geometry::Vec3D zSlope;       ///< Z- normal to beam
  Geometry::Vec3D targetPt;     ///< Exit point at target center
  Geometry::Vec3D frontPt;      ///< Vertex point [Front Face of shutter]
  Geometry::Vec3D endPt;        ///< Exit point of shutter (centre)

  // Cells:
  int upperCell;                ///< Block that stops the beam
  int lowerCell;                ///< Block that stops the beam
  int innerVoidCell;            ///< inner void cell


  //--------------
  // FUNCTIONS:
  //--------------

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp*);
  void createSurfaces();
  void createObjects(Simulation&);
  std::string divideStr() const;
  
  void createStopBlocks(Simulation&,const size_t);
  void createCutUnit(Simulation&,const std::string&);
  void createBlocks(Simulation&);
  void createLinks();
  void applyRotations(const double);

  
 public:

  GeneralShutter(const size_t,const std::string&);
  GeneralShutter(const GeneralShutter&);
  GeneralShutter& operator=(const GeneralShutter&);
  virtual ~GeneralShutter();

  /// Access shutter number
  size_t getShutterNumber() const { return shutterNumber; }
  /// Set closed/open
  void setClosed(const int C) { closed=C; }

  /// Access plane
  const Geometry::Plane* getDPlane() const { return DPlane; }
  /// Access Origin
  const Geometry::Vec3D& getOrigin() const { return Origin; }
  /// Get target Center
  Geometry::Vec3D getTargetPoint() const { return targetPt; }
  /// Get View origin
  Geometry::Vec3D getViewOrigin() const;

  /// Access FrontPoint
  Geometry::Vec3D getFrontPt() const 
    { return getLinkPt(1); }

  /// Access BackPoint
  virtual Geometry::Vec3D getBackPt() const 
    { return getLinkPt(2); }
  
  /// Access flat-angle
  double getAngle() const { return xyAngle; }
  /// Access divide surface 
  int getDivideSurf() const { return divideSurf; }
  /// Access Axis
  const Geometry::Vec3D& getXYAxis() const { return XYAxis; }
  /// Access Beam Axis
  const Geometry::Vec3D& getBeamAxis() const { return BeamAxis; }
  /// Access outer limit
  double getORadius() const { return outerRadius; }
  
  void setDivide(const int);
  void setGlobalVariables(const double,const double,
			  const double,const double);
  void setExternal(const int,const int,const int,const int);
  virtual void createAll(Simulation&,const double,
			 const attachSystem::FixedComp*);

  virtual int exitWindow(const double,std::vector<int>&,
			 Geometry::Vec3D&) const;
}; 

}

#endif
 
