/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsTubeCable.h
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
#ifndef xraySystem_cosaxsTubeCable_h
#define xraySystem_cosaxsTubeCable_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsTubeCable
  \version 1.0
  \author Konstantin Batkov
  \date 1 Jul 2019
  \brief Folding cables in the CoSAXS tube
*/

class cosaxsTubeCable :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  double zStep;                 ///< Cable offset along vertical axis
  double tailRadius;            ///< Tail radius
  double detYStep;              ///< Detector offset along the beam
  int    mat;                   ///< Material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsTubeCable(const std::string&);
  cosaxsTubeCable(const cosaxsTubeCable&);
  cosaxsTubeCable& operator=(const cosaxsTubeCable&);
  virtual cosaxsTubeCable* clone() const;
  virtual ~cosaxsTubeCable();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


