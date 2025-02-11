/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   System/constructInc/torusUnit.h
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
#ifndef constructSystem_torusUnit_h
#define constructSystem_torusUnit_h

class Simulation;

namespace constructSystem
{

/*!
  \class torusUnit
  \version 1.0
  \author Konstantin Batkov
  \date November 2025
  \brief Torus approximated with concentric cones and cylinders
*/

class torusUnit : public attachSystem::ContainedComp,
                    public attachSystem::FixedRotate,
                    public attachSystem::CellMap,
                    public attachSystem::SurfMap
{
 private:

  double rMinor;             ///< minor radius
  double rMajor;             ///< major radius
  int nFaces;                ///< number of the inscribed polygon faces

  int mat;                   ///< Material

  std::vector<std::pair<double, double>> getVertices() const;
  double getApex(const std::pair<double, double>&, const std::pair<double, double>&) const;

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  torusUnit(const std::string&);
  torusUnit(const torusUnit&);
  torusUnit& operator=(const torusUnit&);
  virtual torusUnit* clone() const;
  virtual ~torusUnit();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
