/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModelInc/ProtonFlight.h
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
#ifndef lensSystem_ProtonFlight_h
#define lensSystem_ProtonFlight_h

class Simulation;

namespace lensSystem
{
/*!
  \class ProtonFlight
  \version 1.0
  \author S. Ansell
  \date January 2010
  \brief Adds the ChipIR Guide
*/

class ProtonFlight :  public attachSystem::ContainedGroup,
  public attachSystem::FixedComp
{
 private:
  
  const int protonIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index

  Geometry::Vec3D boxX;         ///< Box X
  Geometry::Vec3D boxY;         ///< Box Y
  Geometry::Vec3D boxZ;         ///< Box Z

  int backSurf;               ///< back surface

  int targetCell;             ///< Cell for the target

  double Angle;               ///< Angle
  double YOffset;             ///< Offset of centre point

  double width;               ///< Top roof height
  double height;              ///< Base (floor)

  double targetBeThick;       ///< Target Be thickness
  double targetBeWidth;       ///< Target Be thickness
  double targetBeHeight;      ///< Target Be thickness
  double targetWaterThick;    ///< Target Water thickness
  double targetWaterWidth;    ///< Target Water thickness
  double targetWaterHeight;    ///< Target Water thickness
  double targetHeight;        ///< Target Height
  double targetThick;         ///< Target Thickness
  double targetWidth;         ///< Target Width

  int targetMat;              ///< Target material
  int targetCoolant;          ///< Target coolant
  int targetSurround;         ///< Target surround mat

  double wallThick;           ///< Wall thickness
  int wallMat;                ///< Material for roof
  
  double protonEnergy;        ///< Proton Energy

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createTarget(Simulation&);

 public:

  ProtonFlight(const std::string&);
  ProtonFlight(const ProtonFlight&);
  ProtonFlight& operator=(const ProtonFlight&);
  ~ProtonFlight();

  void addBack(const int N) { backSurf=N; }         ///< Back surf

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
