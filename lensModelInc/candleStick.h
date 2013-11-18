/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModelInc/candleStick.h
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
#ifndef candleStick_h
#define candleStick_h

class Simulation;

namespace lensSystem
{

class siModerator;

/*!
  \class candleStick
  \version 1.0
  \author S. Ansell
  \date November 2009
  \brief Candlestick an vacuum space to hold a moderator 
*/

class candleStick :public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int surIndex;           ///< Index of surface offset
  int cellIndex;                ///< Index of cell offset
  int populated;                ///< populated or not

  attachSystem::ContainedComp TopBoundary;     ///< Boundary for beam
  attachSystem::ContainedComp HeadBoundary;    ///< Boundary connectin

  Geometry::Vec3D BaseCent;     ///< Central point of base
  Geometry::Vec3D StickCent;    ///< Central point of stick support
  Geometry::Vec3D CylCent;      ///< Circle centre of support

  double baseThick;        ///< BaseThickness
  double baseWidth;        ///< Base Width (beamdirection)
  double baseLength;       ///< Base Length
  double baseXoffset;      ///< X-Offset of base to moderator
  double baseYoffset;      ///< Y-Offset of base to moderator

  double vsWidth;          ///< Width (beam) thickness
  double vsDepth;          ///< vertial xbeam thickness
  double vsHeight;         ///< vertial height
  double vsXoffset;        ///< X-Offset of support [Def: Ddge pos]
  double vsYoffset;        ///< Y-Offset of supgport [Def: Edge Pos]

  double flatWidth;       ///< Flat width [def vsWidth]
  double flatLength;      ///< length extention
  double flatThick;       ///< Thickness since a flat object
  
  double vacLowMX;           ///< lower +ve x (x-Beam)
  double vacLowPX;           ///< lower -ve x (x-Beam)
  double vacLowMY;           ///< lower +ve y (beam)
  double vacLowPY;           ///< lower -ve y (beam)
  double vacBase;            ///< Base of vac
  double vacTop;             ///< Base of vac
  double vacZExt;            ///< Cut mid up
  double vacRadius;          ///< Radius of round locator

  double xCyl;               ///< X-Offset of master cylinder
  double yCyl;               ///< Y-Offset of master cylinder

  double alTopThick;         ///< Al Top Thickness
  double alSideThick;        ///< Al Side Thickness
  double alBaseThick;        ///< Al Base Thickness

  double skinForwardClear;   ///< Forward beam clearance
  double skinBaseClear;      ///< Base clearance
  double skinBackClear;      ///< Back clearance - Towards target
  double skinPSideClear;     ///< Positive side clearance 
  double skinMSideClear;     ///< Negative side clearance
  double skinCutBaseClear;   ///< CutSection base clear
  double skinCircleClear;    ///< Circle clearance
  double skinCutSideClear;   ///< Cut Side clearance

  double waterAlThick;       ///< Water al thickness
  double waterHeight;        ///< Water reflector height

  int supportMat;            ///< Material for support
  int alMat;                 ///< Material for Al surround
  
  int outerDepth;            ///< Outer depth values

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);
  void createLinks();

 public:

  candleStick(const std::string&);
  candleStick(const candleStick&);
  candleStick& operator=(const candleStick&);
  ~candleStick() {}  ///< Destructor

  /// Get Top boundary object:
  std::string getTopExclude() const 
    { return TopBoundary.getExclude(); }

  /// Get Head boundary object:
  std::string getHeadExclude() const 
    { return HeadBoundary.getExclude(); }

  void createAll(Simulation&,const siModerator&);
  void specialExclude(Simulation&,const int) const;
};

}


#endif
 
