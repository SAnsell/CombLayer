/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   insertUnitInc/insertPencil.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell and Konstantin Batkov
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
#ifndef insertSystem_insertPencil_h
#define insertSystem_insertPencil_h

class Simulation;

namespace insertSystem
{
/*!
  \class insertPencil
  \version 1.0
  \author K. Batkov
  \date August 2025
  \brief Pencil inserted in object

  Designed as a temporary bremsstrahlung/synchrotron target for hutch
  wall design. The tip angle can be varied to study scattering
  effects.
*/

class insertPencil : public insertSystem::insertObject
{
 private:

  double radius;             ///< Main radius
  double length;             ///< total length
  double angle;              ///< full tip angle
  int voidMat;               ///< material outside the tip

  void populate(const FuncDataBase&) override;

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void mainAll(Simulation&);

 public:

  insertPencil(const std::string&);
  insertPencil(const std::string&,const std::string&);
  insertPencil(const insertPencil&);
  insertPencil& operator=(const insertPencil&);
  ~insertPencil() override;

  void setValues(const double,const double,const double,const int,const int);
  void setValues(const double,const double,const double,const std::string&,const std::string&);
  void createAll(Simulation&,const Geometry::Vec3D&,
		 const Geometry::Vec3D&);

  void createAll(Simulation&,const Geometry::Vec3D&,
		 const attachSystem::FixedComp&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
