/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/LineShield.h
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
#ifndef constructSystem_LineShield_h
#define constructSystem_LineShield_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class LineShield
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief LineShield unit  
*/

class LineShield :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  const int shieldIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  bool activeFront;             ///< Flag for front active
  bool activeBack;              ///< Flag for back active

  HeadRule frontSurf;           ///< Front surfaces [if used]
  HeadRule frontCut;            ///< front divider surfaces [if used]
  HeadRule backSurf;            ///< Back surfaces [if used]
  HeadRule backCut;             ///< Back surfaces [if used]
  
  double length;                ///< void length [total]
  double left;                  ///< total left width
  double right;                 ///< total right width
  double height;                ///< Total height
  double depth;                 ///< Total depth

  int defMat;                    ///< Fe material layer

  size_t nSeg;                   ///< number of segments
  // Layers
  size_t nWallLayers;               ///< number of layers
  std::vector<double> wallFrac;     ///< wall Layer thicknesss 
  std::vector<int> wallMat;         ///< wall Layer materials

  size_t nRoofLayers;                ///< number of layers
  std::vector<double> roofFrac;      ///< Layer thicknesss 
  std::vector<int> roofMat;          ///< Layer materials

  size_t nFloorLayers;                ///< number of layers
  std::vector<double> floorFrac;      ///< Layer thicknesss 
  std::vector<int> floorMat;          ///< Layer materials


  void removeFrontOverLap();
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  LineShield(const std::string&);
  LineShield(const LineShield&);
  LineShield& operator=(const LineShield&);
  virtual ~LineShield();


  HeadRule getXSectionIn() const;
  void setFront(const attachSystem::FixedComp&,const long int);
  void setBack(const attachSystem::FixedComp&,const long int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
