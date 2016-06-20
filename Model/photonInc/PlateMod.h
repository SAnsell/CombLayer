/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/PlateMod.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef photonSystem_PlateMod_h
#define photonSystem_PlateMod_h

class Simulation;

namespace photonSystem
{

struct plateInfo
{
  double thick;                ///< Thickness
  double vHeight;              ///< void height
  double vWidth;               ///< void width
  int mat;                     ///< Material
  double temp;                 ///< temperature [K]
};

/*!
  \class PlateMod
  \author S. Ansell
  \version 1.0
  \date June 2016
  \brief Specialized for a simple plate object
*/

class PlateMod : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const int plateIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double outerWidth;                ///< Outer width
  double outerHeight;               ///< Outer Height
  
  double innerWidth;                ///< inner width 
  double innerHeight;               ///< inner height

  std::vector<plateInfo> Layer;    ///< Layer info

  int outerMat;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  PlateMod(const std::string&);
  PlateMod(const PlateMod&);
  PlateMod& operator=(const PlateMod&);
  virtual ~PlateMod();
  virtual PlateMod* clone() const;
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
