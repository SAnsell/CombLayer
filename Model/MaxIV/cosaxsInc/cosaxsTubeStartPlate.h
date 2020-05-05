/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsTubeStartPlate.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef xraySystem_cosaxsTubeStartPlate_h
#define xraySystem_cosaxsTubeStartPlate_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsTubeStartPlate
  \version 1.0
  \author Konstantin Batkov
  \date 3 Jun 2019
  \brief Start plate of the Submarine
*/

class cosaxsTubeStartPlate :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double thick;                ///< Total thick
  double radius;                 ///< Radius
  double portRadius;                ///< PortRadius

  int mat;                  ///< Plate material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsTubeStartPlate(const std::string&);
  cosaxsTubeStartPlate(const cosaxsTubeStartPlate&);
  cosaxsTubeStartPlate& operator=(const cosaxsTubeStartPlate&);
  virtual cosaxsTubeStartPlate* clone() const;
  virtual ~cosaxsTubeStartPlate();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


