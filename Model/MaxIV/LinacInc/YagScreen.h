/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/YagScreen.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef tdcSystem_YagScreen_h
#define tdcSystem_YagScreen_h

class Simulation;

namespace tdcSystem
{

/*!
  \class YagScreen
  \version 2.0
  \author Konstantin Batkov
  \date May 2020
  \brief Yag screen
*/

class YagScreen :
    public attachSystem::ContainedGroup,
    public attachSystem::FixedRotate,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:

  double juncBoxLength;         ///< electronics junction box length
  double juncBoxWidth;          ///< electronics junction box width
  double juncBoxHeight;         ///< electronics junction box height
  double juncBoxWallThick;      ///< electronics junction box wall thickness


  double feedLength;            ///< linear pneumatics feedthrough length
  double feedInnerRadius;       ///< linear pneumatics feedthrough inner radius
  double feedWallThick;         ///< linear pneumatics feedthrough wall thickness
  double feedFlangeLen;         ///< linear pneumatics feedthrough flange length
  double feedFlangeRadius;      ///< linear pneumatics feedthrough flange radius

  double threadLift;            ///< screen thread lift
  double threadRadius;          ///< screen thread inner radius

  // volume containing Yag screen + mirror

  double holderWidth;           ///< Flat across holder
  double holderDepth;           ///< In beamaxis
  double holderShortLen;         ///< short length of trapizium
  double holderLongLen;         ///< long length

  double mirrorAngle;           ///< Flat (mirror) angle
  double mirrorRadius;          ///< quartz mirror radius
  double mirrorThick;           ///< quartz mirror thickness

  double screenAngle;           ///< screen inclination angle [deg]
  double screenVOffset;         ///< screen vertical offset
  double screenRadius;          ///< Radius of screen
  double screenThick;           ///< Thickness of screen

  double screenHolderRadius;    ///< screen holder thickness
  double screenHolderThick;     ///< screen holder thickness

  int voidMat;                  ///< void material

  /// electronics junction box cable/inside material
  int juncBoxMat;                
  int juncBoxWallMat;        ///< electronics junction box wall material
  int threadMat;             ///< screen thread material
                            
  int holderMat;            ///< screen holder material
  int mirrorMat;            ///< mirror  material
  int screenMat;            ///< screen  material
  int screenHolderMat;      ///< screen holder  material
  int feedWallMat;          ///< Feedthrough wall material  

  // Geometry points for object / surface building :
  
  Geometry::Vec3D mirrorCentre;        ///< central axis with beam point
  Geometry::Vec3D mirrorStart;         ///< Cut start point 
  Geometry::Vec3D mirrorImpact;        ///< Mirror-beam impact point  
  Geometry::Vec3D screenImpact;        ///< Screen-beam impact point
  Geometry::Vec3D threadEnd;           ///< Thread end point


  bool inBeam;                ///< screen and mirror are in the beam

  /// Norminal line to get screen centre 
  Geometry::Line beamAxis;

  void calcImpactVector();

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  YagScreen(const std::string&);
  YagScreen(const YagScreen&);
  YagScreen& operator=(const YagScreen&);
  virtual ~YagScreen();

  void setBeamAxis(const attachSystem::FixedComp&,const long int);
  void setBeamAxis(const Geometry::Vec3D&,const Geometry::Vec3D&);

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
