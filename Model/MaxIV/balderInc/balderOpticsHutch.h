/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   balderInc/bladerOpticsHutch.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_bladerOpticsHutch_h
#define xraySystem_balderOpticsHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;
/*!
  \class balderOpticsHutch
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief balderOpticsHutch unit

  Built around the central beam axis
*/

class balderOpticsHutch :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double height;                ///< void height
  double length;                ///< void out side width
  double ringWidth;             ///< void flat part to ring
  double ringWallLen;           ///< void flat length [before angle]
  double ringWallAngle;         ///< angle on ring side wall

  double ringConcThick;         ///< Thickness of concreate on ring side

  double outWidth;              ///< out side width
  double innerThick;            ///< Inner wall/roof skin
  double pbWallThick;           ///< Thickness of lead in walls
  double pbFrontThick;          ///< Thickness of lead in front plate
  double pbBackThick;           ///< Thickness of lead in back plate
  double pbRoofThick;           ///< Thickness of lead in Roof
  double outerThick;            ///< Outer wall/roof skin

  double innerOutVoid;          ///< Extension for inner left void space
  double outerOutVoid;          ///< Extension for outer left void space
  double extension;             ///< Extension for void behind back wall

  double inletXStep;            ///< Inlet XStep
  double inletZStep;            ///< Inlet ZStep
  double inletRadius;           ///< Inlet radius

  double holeXStep;            ///< Hole XStep
  double holeZStep;            ///< Hole ZStep
  double holeRadius;           ///< Hole radius

  int skinMat;                ///< Fe layer material for walls
  int ringMat;                ///< Conc layer material for ring walls
  int pbMat;                  ///< pb layer material for walls

  double beamTubeRadius;      ///< Void to construct components in

  /// Chicanes
  std::vector<std::shared_ptr<PortChicane>> PChicane;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void createChicane(Simulation&);

 public:

  balderOpticsHutch(const std::string&);
  balderOpticsHutch(const balderOpticsHutch&);
  balderOpticsHutch& operator=(const balderOpticsHutch&);
  virtual ~balderOpticsHutch();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
