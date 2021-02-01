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
#ifndef cryoSystem_cryoSample_h
#define cryoSystem_cryoSample_h

class Simulation;

namespace cryoSystem
{
  
/*!
  \class CryoMagnetBase
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Sample in a cylindrical sample holder
*/

class cryoSample :  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  //  const int layerIndex;         ///< Index of surface offset
  //  int cellIndex;                ///< Cell index

  double sampleRad; //sample radius
  double sampleHei; //sample height
  double sTemp;//sample temperature
  int smat;//sample material
  
  double holderThickness; //thickness of a holder cylinder
  double holderOuterRadius; //outer radius of a sample holder

  double holderTop;//distance between sample position and holder top
  double holderBott;//distance between sample position and holder bottom 

  double hTemp; // holder temperature
  int hmat; //holder material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  cryoSample(const std::string&);
  cryoSample(const cryoSample&);
  cryoSample& operator=(const cryoSample&);
  virtual ~cryoSample();
  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
