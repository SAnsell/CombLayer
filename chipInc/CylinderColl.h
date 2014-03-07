/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/CylinderColl.h
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
#ifndef ShutterSystem_CylinderColl_h
#define ShutterSystem_CylinderColl_h

class Simulation;

namespace shutterSystem
{
  class shutterInfo;
  class GeneralShutter;

/*!
  \class CylinderColl
  \author S. Ansell
  \version 1.0
  \date Septebmer 2012
  \brief Specialized for for the ChipIRInsert
*/

class CylinderColl : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const std::string keyName;  ///< KeyName
  const int trackIndex;       ///< Index of surface offset
  int cellIndex;              ///< cell index

  Geometry::Vec3D voidOrigin; ///< Void origin

  size_t nCyl;                ///< Number of cylinders
  /// Track of object
  std::vector<ModelSupport::cylTrack> CVec;

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::TwinComp&);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  CylinderColl(const std::string&);
  CylinderColl(const CylinderColl&);
  CylinderColl& operator=(const CylinderColl&);
  virtual ~CylinderColl();

  void createAll(Simulation&,const attachSystem::TwinComp&);
  
};

}

#endif
 
