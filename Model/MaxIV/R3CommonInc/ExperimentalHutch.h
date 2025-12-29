/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   R3CommonInc/ExperimentalHutch.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell & Konstantin Batkov
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
#ifndef xraySystem_ExperimentalHutch_h
#define xraySystem_ExperimentalHutch_h

class Simulation;

namespace xraySystem
{
  class PortChicane;

/*!
  \class ExperimentalHutch
  \version 1.1
  \author S. Ansell & K. Batkov
  \date December 2025
  \brief ExperimentalHutch unit

  Built around the central beam axis
*/

class ExperimentalHutch :
  public xraySystem::XRayHutchBase,
  public attachSystem::ContainedComp
{
 private:
  double ringWidth;             ///< void flat part to ring

  double cornerAngle;           ///< angle at corner
  double cornerLength;          ///< length to corner

  double pbTiltedThick;         ///< Thickness of lead in tilted wall
  double pbFrontThick;          ///< Thickness of lead in front plate(if used)

  double fHoleRadius;            ///< Radius of front hole (if used)
  double fHoleXStep;             ///< X step offset
  double fHoleZStep;             ///< Z step offset

  double frontVoid;             ///< Extension for inner front void space
  double backVoid;              ///< Extension for inner back void space

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createChicane(Simulation&);
  void createForkCut(Simulation&);

 public:

  ExperimentalHutch(const std::string&);
  ExperimentalHutch(const ExperimentalHutch&);
  ExperimentalHutch& operator=(const ExperimentalHutch&);
  ~ExperimentalHutch() override;

  const PortChicane* getPortItem(const size_t) const;

  void splitChicane(Simulation& System,const size_t,const size_t);
  using FixedComp::createAll;
  void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
