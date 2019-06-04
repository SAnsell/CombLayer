/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/cosaxsSampleArea.h
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
#ifndef xraySystem_cosaxsSampleArea_h
#define xraySystem_cosaxsSampleArea_h

class Simulation;

namespace xraySystem
{

/*!
  \class cosaxsSampleArea
  \version 1.0
  \author Konstantin Batkov
  \date 15 May 2019
  \brief Sample area box
*/

class cosaxsSampleArea :
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
  double depth;                 ///< Depth

  int airMat;                   ///< Air material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  cosaxsSampleArea(const std::string&);
  cosaxsSampleArea(const cosaxsSampleArea&);
  cosaxsSampleArea& operator=(const cosaxsSampleArea&);
  virtual cosaxsSampleArea* clone() const;
  virtual ~cosaxsSampleArea();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


