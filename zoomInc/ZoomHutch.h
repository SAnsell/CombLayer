/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomHutch.h
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
#ifndef zoomSystem_ZoomHutch_h
#define zoomSystem_ZoomHutch_h

class Simulation;

namespace zoomSystem
{

/*!
  \class ZoomHutch
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief ZoomHutch [insert object]
*/

class ZoomHutch : public attachSystem::ContainedComp,
    public attachSystem::TwinComp
{
 private:
  
  const int hutchIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var
  
  ZoomTank tank;                ///< Tank object

  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target
  double zStep;                 ///< Offset on Z top Target
  double xyAngle;               ///< x-y Rotation angle
  double zAngle;                ///< z Rotation angle
  
  double frontLeftWidth;        ///< left side width [1st]
  double midLeftWidth;          ///< left side width [2nd]
  double backLeftWidth;         ///< left side width [3rd]
  double frontRightWidth;       ///< right side width [1st]
  double midRightWidth;         ///< right side width [2nd]
  double backRightWidth;        ///< right side width [3rd]

  double frontLeftLen;          ///< length to front/mid divide
  double midLeftLen;            ///< length to mid/back divide
  double frontRightLen;         ///< length to front/mid divide
  double midRightLen;           ///< length to mid/back divide

  double portRadius;            ///< Beamport radius

  double fullLen;               ///< Full Length
  double wallThick;             ///< Wall thickness
  double floorThick;            ///< Floor thickness
  double roofThick;             ///< Rood thickness
  double floorDepth;            ///< Depth below beam entrance
  double roofHeight;            ///< height about beam entrance

  int wallMat;                  ///< Material for walls
  int roofMat;                  ///< Material for roof
  int floorMat;                 ///< Material for floor

  int innerVoid;                ///< Inner void cell

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  ZoomHutch(const std::string&);
  ZoomHutch(const ZoomHutch&);
  ZoomHutch& operator=(const ZoomHutch&);
  virtual ~ZoomHutch();

  void createAll(Simulation&,const attachSystem::TwinComp&);

};

}

#endif
 
