/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/t1WaterModerator.h
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
#ifndef moderatorSystem_WaterModerator_h
#define moderatorSystem_WaterModerator_h

class Simulation;


namespace ts1System
{

/*!
  \class t1WaterModerator
  \version 1.0
  \author F. Burge
  \date June 2012
  \brief TS1 t1WaterModerator [insert object]
*/

class t1WaterModerator : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int waterIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle [degrees]
  double xOutSize;              ///< Left/Right size (out)
  double yOutSize;              ///< length down target (out)
  double zOutSize;              ///< Vertical size (out)
  double alThickOut;            ///< Al thickness (out)

  int alMat;                    ///< Al
  int waterMat;                   ///< water

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  t1WaterModerator(const std::string&);
  t1WaterModerator(const t1WaterModerator&);
  t1WaterModerator& operator=(const t1WaterModerator&);
  virtual ~t1WaterModerator();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
