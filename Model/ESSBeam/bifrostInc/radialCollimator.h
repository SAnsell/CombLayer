/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   simpleCave/radialCollimator.h
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
#ifndef simpleCaveSystem_radialCollimator_h
#define simpleCaveSystem_radialCollimator_h

class Simulation;

namespace simpleCaveSystem
{
  
/*!
  \class radialCollimator
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Sample in a cylindrical sample holder
*/

class radialCollimator :  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  //  const int layerIndex;         ///< Index of surface offset
  //  int cellIndex;                ///< Cell index

  double innerRad; //inner radius
  double middleRad; //radius where Be filter starts
  double outerRad; //outer radius
  double height; //height of cutout from the sample environment side
  double bladeAngle; // Angle between blades
  double bladeThick;// Blade thickness
  double startAngle;//starting angle for the colimator
  double endAngle;//end angle for the collimator
  int bladeMat;//blade material
  int filterMat;//cold neutron filter material

  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  //  void layerProcess(Simulation&);

 public:

  radialCollimator(const std::string&);
  radialCollimator(const radialCollimator&);
  radialCollimator& operator=(const radialCollimator&);
  virtual ~radialCollimator();
  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
