/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/WedgeInsert.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef constructSystem_WedgeInsert_h
#define constructSystem_WedgeInsert_h

class Simulation;

namespace constructSystem
{

/*!
  \class WedgeInsert
  \author S. Ansell
  \version 1.0
  \date May 2014
  \brief Places a bound  unit within an object

  It adds a component within any object, e.g. a moderator
*/

class WedgeInsert : public attachSystem::FixedOffset,
  attachSystem::ContainedComp
{
 private:
  
  const std::string baseName;   ///< Base name 
  const int wedgeIndex;         ///< Surface offset
  int cellIndex;                ///< Cell index

  double viewWidth;         ///< View height [inner]
  double viewHeight;        ///< View Width  [inner]
  double viewXY;            ///< Angle [deg] opening
  double viewZ;             ///< Angle [deg] top/base

  double wall;              ///< Wall thickness
  int mat;                  ///< Main Material
  int wallMat;              ///< Material
  double temp;              ///< Temperature [K]

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,
		     const attachSystem::FixedComp&,
		     const size_t,const size_t);
  void createLinks();

 public:

  WedgeInsert(const std::string&,const size_t);
  WedgeInsert(const WedgeInsert&);
  WedgeInsert& operator=(const WedgeInsert&);
  virtual WedgeInsert* clone() const;
  virtual ~WedgeInsert();

  void createAll(Simulation&,const int,
		 const attachSystem::FixedComp&,
		 const size_t,const size_t);
  
};

}

#endif
 
