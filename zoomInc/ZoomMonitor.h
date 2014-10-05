/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomMonitor.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef zoomSystem_ZoomMonitor_h
#define zoomSystem_ZoomMonitor_h

class Simulation;

namespace shutterSystem
{
  class GeneralShutter;
  class BulkShield;
}

namespace zoomSystem
{
  class ZoomBend;

/*!
  \class ZoomMonitor
  \version 1.0
  \author S. Ansell
  \date September 2011
  \brief ZoomMonitor unit  
*/

class ZoomMonitor : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int monIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< Origin step
  double yStep;                 ///< Origin step
  double zStep;                 ///< Origin step
  double xyAngle;               ///< xy angle
  double zAngle;                ///< Z angle
  
  double viewThick;             ///< Thickness of wall
  double wallThick;             ///< Thickness of wall
  double length;                ///< length [beam Y]
  double radius;                ///< Radius

  int mat;                      ///< Material [detection gas]
  int wallMat;                  ///< wallMat outer material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();

  void createObjects(Simulation&);
  void createLinks();

 public:

  ZoomMonitor(const std::string&);
  ZoomMonitor(const ZoomMonitor&);
  ZoomMonitor& operator=(const ZoomMonitor&);
  virtual ~ZoomMonitor();

  /// Set surface
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,
		 const attachSystem::ContainedComp&);


};

}

#endif
 
