/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/Wendi.h
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
#ifndef xraySystem_Wendi_h
#define xraySystem_Wendi_h

class Simulation;

namespace xraySystem
{

/*!
  \class Wendi
  \version 1.0
  \author Konstantin Batkov
  \date February 2024
  \brief WENDI
*/

class Wendi : public attachSystem::ContainedComp,
	      public attachSystem::FixedRotate,
	      public attachSystem::CellMap,
	      public attachSystem::SurfMap
{
 private:

  double radius;                ///< Radius
  double height;                ///< Height

  int mainMat;                   ///< Main material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Wendi(const std::string&);
  Wendi(const Wendi&);
  Wendi& operator=(const Wendi&);
  virtual Wendi* clone() const;
  virtual ~Wendi();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
