/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/BladeBPM.h
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
#ifndef xraySystem_BladeBPM_h
#define xraySystem_BladeBPM_h

class Simulation;

namespace xraySystem
{

/*!
  \class BladeBPM
  \version 1.0
  \author Konstantin Batkov
  \date February 2026
  \brief Photoemission Blade Beam Position Monitor (XBPM)
*/

class BladeBPM :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double wallThick;             ///< Wall thickness

  int mainMat;                  ///< Main material
  int wallMat;                  ///< Wall material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BladeBPM(const std::string&);
  BladeBPM(const BladeBPM&);
  BladeBPM& operator=(const BladeBPM&);
  virtual BladeBPM* clone() const;
  virtual ~BladeBPM();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
