/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muonInc/profileMon.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_profileMon_h
#define muSystem_profileMon_h

class Simulation;

namespace muSystem
{

/*!
  \class profileMon
  \author G. Skoro
  \version 1.0
  \date October 2013
  \brief profile monitor
*/

class profileMon : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int profMonIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step
  double xyAngle;               ///< Angle (rotation)

  double height;                ///< Height
  double depth;                 ///< Depth
  double width;                 ///< Width

  int steelMat;                   ///Material: Steel

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  profileMon(const std::string&);
  profileMon(const profileMon&);
  profileMon& operator=(const profileMon&);
  virtual ~profileMon();
  
  void createAll(Simulation&,const attachSystem::FixedComp&);  
};

}

#endif
 
