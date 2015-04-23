/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/VoidVessel.h
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
#ifndef ShutterSystem_VoidVessel_h
#define ShutterSystem_VoidVessel_h

class Simulation;

namespace shutterSystem
{

/*!
  \class VoidVessel
  \author S. Ansell
  \version 1.0
  \date February 2011
  \brief Specialized for for the VoidVessel
*/

class VoidVessel : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int voidIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  int populated;                  ///< 1:var

  Geometry::Vec3D voidOrigin;     ///< Void vessel centre

  double vXoffset;                ///< Offset of the void vessel centre

  double vThick;                  ///< thickness of walls
  double vGap;                    ///< Gap after void
  double vSide;                   ///< Side [outer dist]
  double vBase;                   ///< Base distance
  double vTop;                    ///< Top distance
  double vBack;                   ///< Back distance
  double vFront;                  ///< EPB distance [outer]

  double vFDepth;                 ///< Depth of forward plates
  double vForwardAngle;           ///< Angle of plates at front

  double vWindowThick;            ///< Thickness of void window
  double vWindowView;             ///< Vertial view size

  double vWindowWTopLen;          ///< West top length
  double vWindowETopLen;          ///< East top length
  double vWindowWLowLen;          ///< West lower length
  double vWindowELowLen;          ///< East lower length

  double vWindowWTopOff;          ///< West top offset
  double vWindowETopOff;          ///< East top offset
  double vWindowWLowOff;          ///< West lower offset
  double vWindowELowOff;          ///< East lower offset

  double vWindowWTopZ;          ///< West top Z height
  double vWindowETopZ;          ///< East top Z height
  double vWindowWLowZ;          ///< West lower Z height
  double vWindowELowZ;          ///< East lower Z height
  
  double vWindowAngleLen;       ///< View port on angles
  double vWindowAngleROff;       ///< Right offset
  double vWindowAngleLOff;       ///< Left offset

  int vWindowMat;                 ///< Void window material
  int vMat;                       ///< Void material

  // Created values:
  int steelCell;                  ///< Cell for the windows

  // Functions:

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp*);
  void createWindows(Simulation&);

 public:

  VoidVessel(const std::string&);
  VoidVessel(const VoidVessel&);
  VoidVessel& operator=(const VoidVessel&);
  virtual ~VoidVessel();

  void processVoid(Simulation&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp* =0);
  
};

}

#endif
 
