/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/NBeamStop.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef tdcSystem_NBeamStop_h
#define tdcSystem_NBeamStop_h

class Simulation;

namespace tdcSystem
{

struct coreUnit
{
  double radius;   // radius of segement

  std::vector<double> length;
  std::vector<int> mat;

  coreUnit(const double R) : radius(R) {}
  coreUnit(const coreUnit&);
  coreUnit& operator=(const coreUnit&);
  ~coreUnit() {}

  void addUnit(const double,const int);
  void addMat(const int);
};
  
/*!
  \class NBeamStop
  \version 1.0
  \author Stuart Ansell
  \date November 2020
  \brief Simple BeamStop for TDC
*/

class NBeamStop :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  size_t nLayers;                 ///< number of layers
  double fullLength;              ///< Total length including voids
  double outerRadius;             ///< Total radius
  std::vector<coreUnit> units;    ///< different length        

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  NBeamStop(const std::string&);
  NBeamStop(const NBeamStop&);
  NBeamStop& operator=(const NBeamStop&);
  virtual ~NBeamStop();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
