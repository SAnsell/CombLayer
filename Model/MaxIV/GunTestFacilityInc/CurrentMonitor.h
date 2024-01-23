/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/CurrentMonitor.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef xraySystem_CurrentMonitor_h
#define xraySystem_CurrentMonitor_h

class Simulation;

namespace xraySystem
{

/*!
  \class CurrentMonitor
  \version 1.0
  \author Konstantin Batkov
  \date January 2024
  \brief Current Monitor
*/

class CurrentMonitor :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length
  double frontRadius;           ///< Inner radius of the front segment
  double midRadius;             ///< Inner radius of the middle segmen
  double backRadius;            ///< Inner radius of the back segment
  double outerRadius;           ///< Outer radius
  double wallThick;             ///< Front/back segment thickness

  int mainMat;                  ///< Main material
  int wallMat;                  ///< Wall material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CurrentMonitor(const std::string&);
  CurrentMonitor(const CurrentMonitor&);
  CurrentMonitor& operator=(const CurrentMonitor&);
  virtual CurrentMonitor* clone() const;
  virtual ~CurrentMonitor();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
