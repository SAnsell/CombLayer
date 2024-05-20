/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/GTFGateValve.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov and Stuart Ansell
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
#ifndef constructSystem_GTFGateValve_h
#define constructSystem_GTFGateValve_h

class Simulation;

namespace constructSystem
{

/*!
  \class GTFGateValve
  \version 1.0
  \author K. Batkov
  \date December 2023
  \brief Gate valve used at the MAX IV Gun Test Facility. See also: GateValveCube (similar object without clamp and lifting structure)
*/

class GTFGateValve :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Void length
  double width;                 ///< Void width (full)
  double height;                ///< height
  double depth;                 ///< depth

  double wallThick;             ///< Wall thickness
  double portARadius;            ///< Port inner radius (opening)
  double portAThick;             ///< Port outer ring
  double portALen;               ///< Forward step of port

  double portBRadius;            ///< Port inner radius (opening)
  double portBThick;             ///< Port outer ring
  double portBLen;               ///< Forward step of port

  bool closed;                  ///< Shutter closed
  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade total thickness
  double bladeRadius;           ///< moving blade outer radius
  double bladeCutThick;         ///< Thickness of the central part cut
  double bladeScrewHousingRadius; ///< Radius of the central steel cylinder for the screw

  double clampWidth;            ///< Clamp width
  double clampDepth;            ///< Clamp depth
  double clampHeight;           ///< Clamp height
  double clampBulkHeight;       ///< Clamp height before the truncated section
  double clampBaseThick;        ///< Clamp base plate thickness
  double clampBaseWidth;        ///< Clamp base plate width
  double clampTopWidth;         ///< Clamp top truncated part width

  double lsFlangeWidth;         ///< Lifting structure square flange width
  double lsFlangeDepth;         ///< Lifting structure square flange depth
  double lsFlangeHeight;        ///< Lifting structure square flange height
  double lsFlangeHoleRadius;    ///< Lifting structure square flange hole radius
  double lsShaftRadius;         ///< Lifting structure shaft inner radius
  double lsShaftThick;          ///< Lifting structure shaft tube thickness
  double lsShaftLength;         ///< Lifting structure shaft tube total length
  double lsShaftFlangeRadius;   ///< Lifting structure shaft flange outer radius
  double lsShaftFlangeThick;    ///< Lifting structure shaft flange thickness

  int voidMat;                  ///< Void material
  int bladeMat;                 ///< blade material
  int wallMat;                  ///< Pipe material
  int clampMat;                ///< Clamp material
  int lsFlangeMat;              ///< Lifting structure square flange material

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  GTFGateValve(const std::string&);
  GTFGateValve(const GTFGateValve&);
  GTFGateValve& operator=(const GTFGateValve&);
  ~GTFGateValve() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
