/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerRoof.h
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
#ifndef essSystem_BunkerRoof_h
#define essSystem_BunkerRoof_h

class Simulation;

namespace essSystem
{
  class Bunker;
  class BunkerMainWall;
  class BunkerInsert;
  
/*!
  \class BunkerRoof
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bunker roof 
*/

class BunkerRoof : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const std::string baseName;     ///< Bunker base name
  const int roofIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  
  double roofThick;              ///< Roof thickness
  int roofMat;                   ///< Default roof material
  
  // ACTIVE COMPONENTS:
  size_t activeRoof;              ///< active flag for roof segments
  size_t nVert;                   ///< number of layers
  size_t nRadial;                 ///< number of radial layers
  size_t nMedial;                 ///< number of horrizontal layers
  std::vector<double> vert;       ///< Fraction of thickness in Z
  std::vector<double> radial;     ///< Frac of radius (inner to outer)
  std::vector<double> medial;     ///< Frac of medial (left to right)


  // PASSIVE COMPONENTS:
  size_t nBasicVert;                        ///< number of layers
  std::vector<double> basicVert;           ///< Roof fractions
  std::vector<std::string> basicMatVec;            ///< Materials

  // BunkerRoof Material distribution:
  std::string loadFile;            ///< BunkerRoof input file
  std::string outFile;             ///< BunkerRoof output file
  
  std::string divider;             ///< Divider if needed
  int baseSurf;                    ///< Real base surf
  int topSurf;                     ///< Real top surf
  int innerSurf;                   ///< Real inner surf
  int outerSurf;                   ///< Real outer surf
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

 public:

  BunkerRoof(const std::string&);
  BunkerRoof(const BunkerRoof&);
  BunkerRoof& operator=(const BunkerRoof&);
  virtual ~BunkerRoof();

  void setVertSurf(const int,const int);
  void setRadialSurf(const int,const int);
  /// Set the divider string for the inner radius
  void setDivider(const std::string& Str) { divider=Str; }
  
  void createSector(Simulation&,const size_t,const int,
		    const int,const int);
  
  void initialize(const FuncDataBase&,
		  const attachSystem::FixedComp&,
		  const long int);

};

}

#endif
 

