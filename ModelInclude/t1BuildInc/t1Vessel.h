/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/t1Vessel.h
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
#ifndef ShutterSystem_t1Vessel_h
#define ShutterSystem_t1Vessel_h

class Simulation;

namespace shutterSystem
{

/*!
  \class t1Vessel
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for for the ts1 void vessel
*/

class t1Vessel : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int voidIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  int populated;                  ///< 1:var

  Geometry::Vec3D voidOrigin;     ///< Void vessel centre

  double vXoffset;                ///< Offset of the void vessel centre

  double Thick;                  ///< thickness of walls
  double Gap;                    ///< Gap after void
  double Side;                   ///< Side [outer dist]
  double Base;                   ///< Base distance
  double Top;                    ///< Top distance
  double Back;                   ///< Back distance
  double Front;                  ///< EPB distance [outer]

  double FDepth;                 ///< Depth of forward plates
  double ForwardAngle;           ///< Angle of plates at front

  double WindowThick;            ///< Thickness of void window
  double WindowView;             ///< Vertial view size

  double WindowWTopLen;          ///< West top length
  double WindowETopLen;          ///< East top length
  double WindowWLowLen;          ///< West lower length
  double WindowELowLen;          ///< East lower length

  double WindowWTopOff;          ///< West top offset
  double WindowETopOff;          ///< East top offset
  double WindowWLowOff;          ///< West lower offset
  double WindowELowOff;          ///< East lower offset

  double WindowWTopZ;          ///< West top Z height
  double WindowETopZ;          ///< East top Z height
  double WindowWLowZ;          ///< West lower Z height
  double WindowELowZ;          ///< East lower Z height
  
  double WindowAngleLen;       ///< View port on angles
  double WindowAngleROff;       ///< Right offset
  double WindowAngleLOff;       ///< Left offset

  int WindowMat;                 ///< Void window material
  int Mat;                       ///< Void material

  // Created values:
  int steelCell;                  ///< Cell for the windows
  int voidCell;                   ///< Inner void cell

  // Functions:

  void populate(const Simulation&);
  void createUnitVector();

  void createSurfaces();
  void createObjects(Simulation&);
  void createWindows(Simulation&);

 public:

  t1Vessel(const std::string&);
  t1Vessel(const t1Vessel&);
  t1Vessel& operator=(const t1Vessel&);
  virtual ~t1Vessel();

  /// Access void cell
  int getVoidCell() const { return voidCell; }

  void processVoid(Simulation&);
  void createAll(Simulation&);
  
};

}

#endif
 
