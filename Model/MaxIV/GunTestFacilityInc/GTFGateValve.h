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
  \brief Gate valve used at the MAX IV Gun Test Facility
*/

class GTFGateValve :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
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
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius

  bool shieldActive;            ///< Shield active flag
  double shieldWidth;           ///< Shield width
  double shieldDepth;           ///< Shield depth
  double shieldHeight;          ///< Shield height
  double shieldBaseThick;       ///< Shield base plate thickness
  double shieldBaseWidth;       ///< Shield base plate width
  double shieldTopWidth;        ///< Shield top truncated part width

  int voidMat;                  ///< Void material
  int bladeMat;                 ///< blade material
  int wallMat;                  ///< Pipe material
  int shieldMat;                ///< Shield material

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
