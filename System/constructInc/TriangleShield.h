/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/TriangleShield.h
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
#ifndef constructSystem_TriangleShield_h
#define constructSystem_TriangleShield_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class TriangleShield
  \version 1.0
  \author S. Ansell
  \date May 2017
  \brief TriangleShield unit  
*/

class TriangleShield :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:
  
  const int shieldIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index  
  
  double length;                ///< void length [total]
  double leftAngle;             ///< angle for left wall
  double rightAngle;            ///< angle for right wall
  double left;                  ///< total left width
  double right;                 ///< total right width
  double height;                ///< Total height
  double depth;                 ///< Total depth

  double endWall;               ///< Total back wall thick
  double endVoid;               ///< Total back wall thick
  
  int defMat;                       ///< Fe material layer

  size_t nSeg;                      ///< number of segments
  // Layers
  size_t nWallLayers;               ///< number of layers
  std::vector<double> wallFrac;     ///< wall Layer thicknesss 
  std::vector<int> wallMat;         ///< wall Layer materials

  size_t nEndLayers;               ///< number of layers
  std::vector<double> endFrac;     ///< end Layer thicknesss 
  std::vector<int> endMat;         ///< end Layer materials

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

  TriangleShield(const std::string&);
  TriangleShield(const TriangleShield&);
  TriangleShield& operator=(const TriangleShield&);
  virtual ~TriangleShield();


  HeadRule getXSectionIn() const;  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
