/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItem/CryoMagnetBase.h
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
#ifndef simpleCaveSystem_caveDetector_h
#define simpleCaveSystem_caveDetector_h

class Simulation;

namespace simpleCaveSystem
{
  
/*!
  \class CryoMagnetBase
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Sample in a cylindrical sample holder
*/

class caveDetector :  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  //  const int layerIndex;         ///< Index of surface offset
  //  int cellIndex;                ///< Cell index

  double innerRad; //sample radius
  double shieldThick; // thickness of the shielding plate
  double tankIntRad; // detector tank internal radius
  double tankThick; // tank thickness
  double tankExtRad; //external radius of the shielding tank
  double shelfWidth,shelfThick; // shelf width and thickness
  //tank height is calculated as 0.5*tankCutHeight+tankThick
  double tankDepth; // tank depth
  double tankCutHeight; //height of cutout from the sample environment side
  int tankMat;//tank material
  int shieldMat;//shielding plate material

  size_t nShieldLayers;       ///< Layers in shielding plate
  std::vector<double> shieldFracList;
  std::vector<int> shieldMatList; 
  

  size_t nTankLayers;       ///< Layers in tank front wall
  std::vector<double> tankFracList;
  std::vector<int> tankMatList; 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  caveDetector(const std::string&);
  caveDetector(const caveDetector&);
  caveDetector& operator=(const caveDetector&);
  virtual ~caveDetector();
  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
