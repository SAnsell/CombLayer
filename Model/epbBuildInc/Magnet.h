/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   epbBuildInc/Magnet.h
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
#ifndef epbSystem_Magnet_h
#define epbSystem_Magnet_h

class Simulation;


namespace epbSystem
{
/*!
  \class Magnet
  \version 1.0
  \author S. Ansell
  \date September 2013

  \brief Magnet for EPB
*/

class Magnet : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const std::string baseName;   ///< Base key
  const int magIndex;          ///< Index of surface offset
  int cellIndex;               ///< Cell index

  double xStep;                ///< Master offset 
  double yStep;                ///< Master offset 
  double zStep;                ///< Master offset 
  double xyAngle;              ///< Xy rotation
  double zAngle;               ///< Rotation

  size_t segIndex;             ///< Index of sector
  size_t segLen;               ///< Number of segments
  double length;               ///< Full length
  double height;               ///< Full height
  double width;                ///< Full width

  int feMat;                    ///< Iron material

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&);
  void createLinks();

 public:

  Magnet(const std::string&,const size_t);
  Magnet(const Magnet&);
  Magnet& operator=(const Magnet&);
  virtual ~Magnet();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
