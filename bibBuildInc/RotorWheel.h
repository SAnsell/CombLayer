/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuildInc/RotorWheel.h
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
#ifndef bibSystem_bibRotorWheel_h
#define bibSystem_bibRotorWheel_h

class Simulation;

namespace bibSystem
{

/*!
  \class RotorWheel
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Specialized for wheel
*/

class RotorWheel : public attachSystem::ContainedGroup,
    public attachSystem::FixedComp
{
 private:
  
  const int wheelIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy angle
  double zAngle;                  ///< zAngle step

  double radius;                  ///< Radius of wheel
  double waterThick;              ///< Middle water thickness
  double wallThick;               ///< Wall thickness
  double beThick;                 ///< Be thickness
  double beLength;                 ///< Front wall thickness
  double frontVac;                ///< General Vac gap
  double backVac;                 ///< General Vac gap
  double endVac;                  ///< General Vac gap
  double outWallThick;            ///< Outer Wall thick

  double boxFront;                ///< Steel Box Offset
  double boxBack;                 ///< Steel Box Offset 
  double boxIn;                   ///< Steel Box offset [to beam]
  double boxOut;                  ///< Steel Box offset [away from beam]
  double boxTop;                  ///< Steel Box Top
  double boxBase;                 ///< Steel Box Base
  double boxInterior;             ///< Steel box interior depth
  double steelThick;              ///< Steel thickness

  double pbDepth;                 ///< Lead thickness
  double pbThick;                 ///< Lead thickness


  int beMat;                      ///< Be Material    
  int waterMat;                   ///< Water material
  int wallMat;                    ///< Wall material
  int vesselMat;                  ///< Outer Vessel material
  int polyMat;                    ///< shieling material 
  int pbMat;                      ///< gamma shield material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  public:

  RotorWheel(const std::string&);
  RotorWheel(const RotorWheel&);
  RotorWheel& operator=(const RotorWheel&);
  virtual ~RotorWheel();

  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
