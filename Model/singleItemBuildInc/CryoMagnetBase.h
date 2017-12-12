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
#ifndef photonSystem_CryoMagnetBase_h
#define photonSystem_CryoMagnetBase_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class CryoMagnetBase
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Specialized for a layered cylinder pre-mod object
*/

class CryoMagnetBase :  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const int layerIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  size_t nLayers;                    ///< Layer count

  std::vector<double> LRad;         ///< Layer Radius
  std::vector<double> LThick;       ///< Layer thickness
  std::vector<double> LTemp;        ///< Layer temperature
  double outerRadius;

  double topOffset;
  double baseOffset; 
  
  double cutTopAngle;
  double cutBaseAngle;

  double topThick;
  double baseThick; 

  double apertureWidth;
  double apertureHeight;
  
  int mat;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CryoMagnetBase(const std::string&);
  CryoMagnetBase(const CryoMagnetBase&);
  CryoMagnetBase& operator=(const CryoMagnetBase&);
  virtual ~CryoMagnetBase();
  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
