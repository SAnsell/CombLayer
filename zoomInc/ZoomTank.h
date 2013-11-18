/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomTank.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef moderatorSystem_ZoomTank_h
#define moderatorSystem_ZoomTank_h

class Simulation;

namespace zoomSystem
{

/*!
  \class ZoomTank
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief ZoomTank [insert object]
*/

class ZoomTank : public attachSystem::ContainedComp,
    public attachSystem::TwinComp
{
 private:
  
  const int tankIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< x-y Rotation angle
  double zAngle;                ///< z Rotation angle
  
  size_t nCylinder;                ///< Number of pre-cylinders
  double cylThickness;          ///< Cylinder wall thickness
  std::vector<double> CRadius;  ///< Radius 
  std::vector<double> CylDepth; ///< Depth of the cylinder
  std::vector<double> CylX;     ///< X-Shift
  std::vector<double> CylZ;     ///< Z-Shift
  double cylTotalDepth;         ///< Total depth of cylinders
  
  double windowThick;           ///< Window thickness
  double windowRadius;          ///< Window radius

  double width;                 ///< Width tank
  double height;                ///< Height tank
  double length;                ///< Length of main tank
  double wallThick;             ///< Wall thickness

  int wallMat;                  ///< Material for walls
  int windowMat;                  ///< Material for walls

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  ZoomTank(const std::string&);
  ZoomTank(const ZoomTank&);
  ZoomTank& operator=(const ZoomTank&);
  ~ZoomTank();

  void createAll(Simulation&,const attachSystem::TwinComp&);

};

}

#endif
 
