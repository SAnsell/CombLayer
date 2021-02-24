/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/SPFCameraShield.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef tdcSystem_SPFCameraShield_h
#define tdcSystem_SPFCameraShield_h

class Simulation;

namespace tdcSystem
{

/*!
  \class SPFCameraShield
  \version 1.0
  \author Konstantin Batkov
  \date February 2021
  \brief SPF Chamera shield
*/

class SPFCameraShield : public attachSystem::ContainedComp,
			public attachSystem::FixedRotate,
			public attachSystem::CellMap,
			public attachSystem::SurfMap
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double wallThick;             ///< Wall [brick] thickness
  double roofLength;            ///< Roof [brick] length
  double roofAngle;             ///< Roof angle around z axis
  double roofXShift;            ///< Roof x-offset
  double roofYShift;            ///< Roof y-offset

  int mat;                      ///< Shield material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SPFCameraShield(const std::string&);
  SPFCameraShield(const SPFCameraShield&);
  SPFCameraShield& operator=(const SPFCameraShield&);
  virtual SPFCameraShield* clone() const;
  virtual ~SPFCameraShield();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
