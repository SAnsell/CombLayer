/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsTube.h
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#ifndef xraySystem_cosaxsTube_h
#define xraySystem_cosaxsTube_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsTube
  \version 1.0
  \author Konstantin Batkov
  \date 6 May 2019
  \brief CoSAXS Submarine
*/

  class cosaxsTubeNoseCone;
  class cosaxsTubeStartPlate;
  class cosaxsTubeSegment;

class cosaxsTube :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:
  double length;                ///< Total length including void
  double radius;                 ///< Radius
  double outerRadius; ///< Radius of bounding volume
  double outerLength; ///< Length of bounding volume
  double wallThick;             ///< Wall thickness
  size_t nSegments; ///< Number of tube segments

  int mainMat;                  ///< Main material
  int wallMat;                  ///< Wall material

  attachSystem::InnerZone buildZone;

  std::shared_ptr<xraySystem::cosaxsTubeNoseCone> noseCone;
  std::shared_ptr<constructSystem::GateValveCylinder> gateA;
  std::shared_ptr<xraySystem::cosaxsTubeStartPlate> startPlate;
  std::vector< std::shared_ptr<xraySystem::cosaxsTubeSegment> > segments;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsTube(const std::string&);
  cosaxsTube(const cosaxsTube&);
  cosaxsTube& operator=(const cosaxsTube&);
  virtual cosaxsTube* clone() const;
  virtual ~cosaxsTube();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


