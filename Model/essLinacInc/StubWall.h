/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/StubWall.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_StubWall_h
#define essSystem_StubWall_h

class Simulation;

namespace essSystem
{

/*!
  \class StubWall
  \version 1.0
  \author Konstantin Batkov
  \date 23 May 2018
  \brief Stub shielding wall in Klystron Galery
*/

class StubWall : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const std::string baseName;   ///< Base name
  const size_t Index;           ///< Wall index
  const int surfIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height
  size_t nLayers;               ///< Number of layers for variance reduction

  int wallMat;                  ///< Wall material
  int airMat;                   ///< Air material

  void layerProcess(Simulation& System, const std::string& cellName,
		    const long int& lpS, const long int& lsS, const size_t&,
		    const int&);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const long int);
  void createLinks(const attachSystem::FixedComp&,
		   const long int,const long int);

 public:

  StubWall(const std::string&,const std::string&,const size_t&);
  StubWall(const StubWall&);
  StubWall& operator=(const StubWall&);
  virtual StubWall* clone() const;
  virtual ~StubWall();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int,const long int);

};

}

#endif


