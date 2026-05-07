/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/WhiteBeamStop.h
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#ifndef xraySystem_WhiteBeamStop_h
#define xraySystem_WhiteBeamStop_h

class Simulation;

namespace xraySystem
{

/*!
  \class WhiteBeamStop
  \version 1.0
  \author Konstantin Batkov
  \date March 2026
  \brief White Beam Stop (without support and cooling pipes)
*/

class WhiteBeamStop : public attachSystem::ContainedComp,
		      public attachSystem::FixedRotate,
		      public attachSystem::CellMap,
		      public attachSystem::SurfMap
{
 private:

  double length;                ///< Angle-invariant length
  double width;                 ///< Angle-invariant width
  double height;                ///< Height
  double angle;                 ///< Angle between the incident beam and the beam target surface (#3)

  int mat;                      ///< Beam stop material

  attachSystem::FixedComp* bsCol; ///< Bremsstrahlung collimator
  std::string bsSide; ///< Bremsstrahlung collimator aperture link name

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  WhiteBeamStop(const std::string&);
  WhiteBeamStop(const WhiteBeamStop&);
  WhiteBeamStop& operator=(const WhiteBeamStop&);
  virtual WhiteBeamStop* clone() const;
  virtual ~WhiteBeamStop();

  using attachSystem::FixedComp::createAll;
  void setCollimator(Simulation&,attachSystem::FixedComp&,const std::string&);
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
