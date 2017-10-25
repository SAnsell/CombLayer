/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/WallCut.h
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
#ifndef constructSystem_WallCut_h
#define constructSystem_WallCut_h

class Simulation;

namespace constructSystem
{

/*!
  \class WallCut
  \version 1.0
  \author S. Ansell
  \date Febrary 2015
  \brief WallCut [insert object]

  Designed as a cutout of a wall to do gaps etc
*/

class WallCut : public attachSystem::FixedOffset,
  public attachSystem::ContainedComp
{
 private:
  
  const int cutIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  const std::string baseName;   ///< Base name

  std::string insertKey;        ///< Insert cell name
    
  double height;                  ///< Height of the cut
  double width;                   ///< Width of the basic cut
  double length;                  ///< Length of basic cut

  int mat;                        ///< Material  [typcially void]
  double matTemp;                 ///< Material Temp
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&,const HeadRule&);
  void createLinks(const HeadRule&);
  
 public:

  WallCut(const std::string&,const size_t);
  WallCut(const WallCut&);
  WallCut& operator=(const WallCut&);
  ~WallCut();

  void populateKey(const FuncDataBase&);
  /// Access to insert key
  const std::string& getInsertKey() const { return insertKey; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const HeadRule&);

};

}

#endif
 
