/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerWall.h
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
#ifndef essSystem_BunkerWall_h
#define essSystem_BunkerWall_h

class Simulation;

namespace essSystem
{
  class Bunker;
  class BunkerMainWall;
  class BunkerInsert;
  
/*!
  \class BunkerWall
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bunker wall 
*/

class BunkerWall : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const std::string baseName;     ///< Bunker base name
  const int wallIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  
  double wallThick;              ///< Wall thickness
  int wallMat;                   ///< Default wall material
  
  // ACTIVE COMPONENTS:
  size_t activeWall;              ///< active flag for wall segments
  size_t nVert;                   ///< number of vert layers
  size_t nRadial;                 ///< number of radial layers
  size_t nMedial;                 ///< number of horrizontal layers
  std::vector<double> vert;       ///< Fraction of thickness in Z
  std::vector<double> radial;     ///< Frac of radius (inner to outer)
  std::vector<double> medial;     ///< Frac of medial (left to right)


  // PASSIVE COMPONENTS:
  size_t nBasic;                            ///< number of layers (rings)
  std::vector<double> basic;                ///< Wall fractions
  std::vector<std::string> basicMatVec;     ///< Materials

  // BunkerWall Material distribution:
  std::string loadFile;            ///< BunkerWall input file
  std::string outFile;             ///< BunkerWall output file
  
  std::string divider;              ///< Divider if needed
  int frontSurf;                    ///< Real base surf
  int backSurf;                     ///< Real top surf
  int topSurf;                      ///< Real inner surf
  int baseSurf;                     ///< Real outer surf
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  BunkerWall(const std::string&);
  BunkerWall(const BunkerWall&);
  BunkerWall& operator=(const BunkerWall&);
  virtual ~BunkerWall();

  void setVertSurf(const int,const int);
  void setRadialSurf(const int,const int);
  void setDivider(const std::string& Str) { divider=Str; }
  
  void createSector(Simulation&,const size_t,const int,
		    const int,const int);
  
  void initialize(const FuncDataBase&,
		  const attachSystem::FixedComp&,
		  const long int);

};

}

#endif
 

