/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/BilbaoWheelCassette.h
 *
 * Copyright (c) 2017 by Stuart Ansell / Konstantin Batkov
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
#ifndef essSystem_BilbaoWheelCassette_h
#define essSystem_BilbaoWheelCassette_h

class Simulation;

namespace essSystem
{

/*!
  \class BilbaoWheelCassette
  \version 1.0
  \author Konstantin Batkov
  \date 21 Aug 2017
  \brief Bilbao wheel sector cassette
*/

class BilbaoWheelCassette : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const std::string baseName;   ///< Base Name
  const std::string commonName; ///< Template (part between wheel name and sector number)
  const int surfIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag
  bool bricksActive;            ///< True if bricks are active

  double wallThick;             ///< Side wall thickness
  double delta;                 ///< Angular width [deg]
  double temp;                  ///< Temperature

  size_t   nWallSeg;            ///< Number of wall segments (for detailed geometry)
  std::vector<double> wallSegLength; ///< Array of wall lengths (for detailed wall geometry)
  double wallSegDelta;          ///< Wall angular width [deg] (for detailed wall geometry)
  double wallSegThick;          ///< Wall base thickness (for detailed wall geometry)

  int mainMat;                  ///< main material
  int wallMat;                  ///< wall material
  size_t floor;                 ///< Floor link point
  size_t roof;                  ///< Roof link point
  size_t back;                  ///< Back cylinder link point
  size_t front;                 ///< Front cylinder link point

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces(const attachSystem::FixedComp&);
  void createSurfacesBricks(const attachSystem::FixedComp&);
  void createObjects(Simulation&,const attachSystem::FixedComp&);
  void createObjectsBricks(Simulation&,const attachSystem::FixedComp&);
  void createLinks();

 public:

  BilbaoWheelCassette(const std::string&,const std::string&,const size_t&);
  BilbaoWheelCassette(const BilbaoWheelCassette&);
  BilbaoWheelCassette& operator=(const BilbaoWheelCassette&);
  virtual BilbaoWheelCassette* clone() const;
  virtual ~BilbaoWheelCassette();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,
		 const size_t,
		 const size_t,
		 const size_t,
		 const size_t,
		 const double&);

};

}

#endif


