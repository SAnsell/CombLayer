/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Model/caveBuildInc/LineShield.h
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
#ifndef constructSystem_Corner_h
#define constructSystem_Corner_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class Corner
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief Corner unit  
*/

class Corner :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  //  const int shieldIndex;        ///< Index of surface offset
  // int cellIndex;                ///< Cell index  

  bool activeFront;             ///< Flag for front active
  bool activeBack;              ///< Flag for back active

  HeadRule frontSurf;           ///< Front surfaces [if used]
  HeadRule frontCut;            ///< front divider surfaces [if used]
  HeadRule backSurf;            ///< Back surfaces [if used]
  HeadRule backCut;             ///< Back surfaces [if used]
  
  double length;                ///< void length [total]
  double front, voidFront; //<Front of the labirynth construction. voidFront==corridor width
  double left,voidLeft;                  ///< total left width, void width left
  double right,voidRight;                 ///< total right width, void width right
  double height,voidHeight;                ///< Total height, void height
  double depth,voidDepth;                 ///< Total depth, void depth
  double spacing;              ///Gap between U-shaped shielding and bottom plate
  double InnerB4CThick;
  double OuterB4CThick;
  
  int defMat;                    ///< Fe material layer
  int voidMat;                    ///< Material in void
  int spacingMat;                    ///< Material in void
  int B4CMat;                    ///Material for B4Ctiles inside and outside
  
  size_t nSeg;                   ///< number of segments
  // Layers
  size_t nWallLayers;               ///< number of layers
  std::vector<double> wallFrac;     ///< wall Layer thicknesss 
  std::vector<int> wallMat;         ///< wall Layer materials

    size_t nFrontLayers;               ///< number of layers
  std::vector<double> frontFrac;     ///< wall Layer thicknesss 
  std::vector<int> frontMat;         ///< wall Layer materials

  size_t nRoofLayers;                ///< number of layers
  std::vector<double> roofFrac;      ///< Layer thicknesss 
  std::vector<int> roofMat;          ///< Layer materials

  size_t nFloorLayers;                ///< number of layers
  std::vector<double> floorFrac;      ///< Layer thicknesss 
  std::vector<int> floorMat;          ///< Layer materials

  int iFrontLining; // whether to construct lining in front of the corner

  void removeFrontOverLap();
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  Corner(const std::string&);
  Corner(const Corner&);
  Corner& operator=(const Corner&);
  virtual ~Corner();


  HeadRule getXSectionIn() const;
  void setFront(const attachSystem::FixedComp&,const long int);
  void setBack(const attachSystem::FixedComp&,const long int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
