/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/EBeamStop.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell / Konstantin Batkov
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
#ifndef tdcSystem_EBeamStop_h
#define tdcSystem_EBeamStop_h

class Simulation;


namespace tdcSystem
{
/*!
  \class EBeamStop
  \version 1.1
  \author S. Ansell / K. Batkov
  \date February 2021

  \brief Electron beam stop for MAX-IV
*/

class EBeamStop :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double width;                 ///< void radius
  double length;                ///< void length [total]
  double height;                ///< void height
  double depth;                 ///< void depth

  double wallThick;             /// main wall thickness
  double baseThick;             /// Base thickness

  double baseFlangeExtra;       ///< base outstep
  double baseFlangeThick;       ///< base vertical rise

  double portLength;            ///< Port length
  double portRadius;            ///< Port radius
  double portWallThick;         ///< Port wall thickness
  double portFlangeRadius;      ///< Port flange radius
  double portFlangeLength;      ///< Port flange length

  double stopRadius;            ///< Main stop radius
  double stopLength;            ///< Main stop length
  double stopZLift;             ///< Lift of support if open

  std::vector<double> stopLen;   ///< Stop material lengths

  double supportConeLen;        ///< Support close cone
  double supportConeRadius;     ///< Support radius at base
  double supportHoleRadius;     ///< Hole in support radius
  double supportRadius;         ///< Radius of

  double stopPortYStep;          ///< Stop port centre YStep
  double stopPortRadius;         ///< stop port radius
  double stopPortLength;         ///< stop port total length
  double stopPortWallThick;      ///< stop port wall radius
  double stopPortDepth;         ///< stop port depth
  double stopPortFlangeRadius;   /// flange radius
  double stopPortFlangeLength;   /// flange length
  double stopPortPlateThick;     /// coverplate thick

  double ionPortYStep;          ///< Y ste
  double ionPortRadius;         ///< ion pump port radius
  double ionPortLength;         ///< ion pump total length
  double ionPortWallThick;      ///< ion pump port wall thick
  double ionPortFlangeRadius;   /// flange radius
  double ionPortFlangeLength;   /// flange length
  double ionPortPlateThick;     /// coverplate thick

  bool closedFlag;              ///< is beamstop closed (in beam)

  std::vector<int> stopMat;     ///< Stop materials

  int voidMat;                  ///< void material
  int wallMat;                ///< flange material
  int flangeMat;                ///< flange material
  int supportMat;                ///< support material
  int plateMat;                 ///< end plate material
  int outerMat;                 ///< pipe material

  bool shieldActive;            ///< Activates outer shielding
  int shieldInnerMat;           ///< Inner layer material
  int shieldOuterMat;           ///< Outer layer material
  int shieldRoofPlateMat;       ///< Material of the plate above the beam stop
  double shieldLength;          ///< Total length
  double shieldWidth;           ///< Total width
  double shieldHeight;          ///< Total height (above beam line)
  double shieldDepth;           ///< Total depth (below beam line)
  double shieldInnerFloorThick; ///< Inner layer floor thickness
  double shieldInnerRoofThick;  ///< Inner layer roof thickness
  double shieldInnerSideThick;  ///< Inner layer side thickness
  double shieldSideHoleWidth;   ///< Side penetration width for the beam pipes
  double shieldSideHoleHeight;  ///< Side penetration height for the beam pipes
  double shieldOuterFloorThick; ///< Outer layer floor thickness
  double shieldOuterSideThick;  ///< Outer layer side thickness
  double shieldOuterRoofThick;  ///< Outer layer roof thickness
  double shieldRoofPlateThick;  ///< Thickness of the plate above the beam stop

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  EBeamStop(const std::string&);
  EBeamStop(const std::string&,const std::string&);
  EBeamStop(const EBeamStop&);
  EBeamStop& operator=(const EBeamStop&);
  virtual ~EBeamStop();

  bool isShieldActive() const { return shieldActive; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
