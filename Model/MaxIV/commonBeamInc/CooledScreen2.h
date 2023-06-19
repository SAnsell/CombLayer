/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/CooledScreen2.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_CooledScreen2_h
#define xraySystem_CooledScreen2_h

class Simulation;

namespace xraySystem
{

/*!
  \class CooledScreen2
  \version 2.0
  \author Stuart Ansell
  \date October 2021
  \brief Cooled Beam Viewer screen
*/

class CooledScreen2 :
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

  double screenAngle;           ///< Y rotation of screen
  double copperWidth;           ///< in-beam copper block width
  double copperHeight;          ///< in-beam copper block height 
  double copperThick;           ///< Thickness of in-beam cooper

  double tubeRadius;            ///< radius of tubing
  double tubeWall;              ///< wall thickness of tubing
  
  double tubeYStep;             ///< y-forward step
  double tubeTopGap;            ///< Gap at the top (free area)
  double tubeMainGap;           ///< Gap in the plate area
  double tubeTopLength;         ///< Length up from the centre point
  double tubeDownLength;        ///< Length down from the centre point

  double innerRadius;           ///< circle radius of beam
  double screenThick;           ///< screen thickness
  
  int voidMat;                  ///< void material

  /// electronics junction box cable/inside material
  int juncBoxMat;                
  int juncBoxWallMat;        ///< electronics junction box wall material
                            
  int copperMat;            ///< screen holder material
  int screenMat;            ///< screen  material

  int feedWallMat;          ///< Feedthrough wall material  

  // Geometry points for object / surface building :
  
  Geometry::Vec3D screenCentre;        ///< central axis with beam point

  bool inBeam;                ///< screen and mirror are in the beam

  /// Norminal line to get screen centre 
  Geometry::Line beamAxis;

  void calcImpactVector();

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CooledScreen2(const std::string&);
  CooledScreen2(const CooledScreen2&);
  CooledScreen2& operator=(const CooledScreen2&);
  ~CooledScreen2() override;

  void setBeamAxis(const attachSystem::FixedComp&,const long int);
  void setBeamAxis(const Geometry::Vec3D&,const Geometry::Vec3D&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
