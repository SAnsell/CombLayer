/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/CompBInsert.h
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
#ifndef essSystem_CompBInsert_h
#define essSystem_CompBInsert_h

class Simulation;

namespace essSystem
{

/*!
  \class CompBInsert
  \version 1.0
  \author S. Ansell
  \date June 2015
  \brief Multi-layer Beam insert
*/

class CompBInsert : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int insIndex;            ///< Index of surface offset
  int cellIndex;                 ///< Cell index

  size_t NBox;                   ///< number of boxes
  std::vector<double> width;     ///< width of each box
  std::vector<double> height;
  std::vector<double> length;
  std::vector<int> mat;

  size_t NWall;                   ///< Number of walls
  std::vector<double> wallThick;  ///< wall thickness
  std::vector<int> wallMat;       ///< Wall materials.
    
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  CompBInsert(const std::string&);
  CompBInsert(const CompBInsert&);
  CompBInsert& operator=(const CompBInsert&);
  virtual ~CompBInsert();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const attachSystem::FixedComp&);

};

}

#endif
 

