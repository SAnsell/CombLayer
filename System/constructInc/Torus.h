/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   System/constructInc/Torus.h
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
#ifndef constructSystem_Torus_h
#define constructSystem_Torus_h

class Simulation;

namespace constructSystem
{

/*!
  \class Torus
  \version 1.0
  \author Konstantin Batkov
  \date November 2024
  \brief Torus approximated with cones and cylinders
*/

class Torus : public attachSystem::ContainedComp,
                    public attachSystem::FixedRotate,
                    public attachSystem::CellMap,
                    public attachSystem::SurfMap
{
 private:

  double rMinor;             ///< minor radius

  int mat;                   ///< Material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Torus(const std::string&);
  Torus(const Torus&);
  Torus& operator=(const Torus&);
  virtual Torus* clone() const;
  virtual ~Torus();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
