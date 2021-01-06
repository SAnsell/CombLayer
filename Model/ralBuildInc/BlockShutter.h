/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/BlockShutter.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef shutterSystem_BlockShutter_h
#define shutterSystem_BlockShutter_h

class Simulation;

/*!
  \namespace BlockShutter System
  \brief Adds Block components in a shutter
  \author S. Ansell
  \version 1.0
  \date September 2009
*/

namespace shutterSystem
{
  class collInsert;

/*!
  \class BlockShutter
  \author S. Ansell
  \version 1.2
  \date March 2010
  \brief Hold the shutter
  Added version to hold multiple InsertBlocks
*/

class BlockShutter : public GeneralShutter
{
 private:

  const int b4cMat;                  ///< Search material for B4C
  std::string blockKey;              ///< Key Name

  int nBlock;                       ///< number of inner blocks    
  double xStep;                     ///< Start Shift
  double xAngle;                    ///< X drop angle
  double zStart;                    ///< Start drop
  double zAngle;                    ///< Z drop angle

  double colletHGap;                 ///< Horrizontal gap
  double colletVGap;                 ///< Vertical gap
  double colletFGap;                 ///< Forward gap

  int colletMat;                     ///< Collet material

  //--------------
  // FUNCTIONS:
  //--------------
  void populate(const FuncDataBase&);
  void createInsert(Simulation&);  
  void createObjects(Simulation&);  
  void createSurfaces();  
  
  double processShutterDrop() const;

 public:

  BlockShutter(const size_t,const std::string&,const std::string&);
  BlockShutter(const BlockShutter&);
  BlockShutter& operator=(const BlockShutter&);
  virtual ~BlockShutter();

  Geometry::Vec3D getExitTrack() const;
  Geometry::Vec3D getExitPoint() const;

  std::vector<Geometry::Vec3D>  createFrontViewPoints() const;
  std::vector<Geometry::Vec3D>  createBackViewPoints() const;

  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 
