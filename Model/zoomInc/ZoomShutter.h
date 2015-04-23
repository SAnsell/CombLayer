/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomShutter.h
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
#ifndef shutterSystem_ZoomShutter_h
#define shutterSystem_ZoomShutter_h

class Simulation;

/*!
  \namespace ZoomShutter Systemx
  \brief Add hutches to the model
  \author S. Ansell
  \version 1.0
  \date September 2009
*/

namespace shutterSystem
{
  class shutterInfo;


/*!
  \class ZoomShutter
  \author S. Ansell
  \version 1.2
  \date March 2010
  \brief Hold the shutter
  Added version to hold multiple zoomInsertBlocks
*/
class ZoomShutter : public GeneralShutter
{
 private:

  std::string zoomKey;              ///< Key Name

  int nBlock;                       ///< number of inner blocks    
  double xStart;                    ///< Start Shift
  double xAngle;                    ///< X drop angle
  double zStart;                    ///< Start drop
  double zAngle;                    ///< Z drop angle

  double colletHGap;                 ///< Horrizontal gap
  double colletVGap;                 ///< Vertical gap
  double colletFGap;                 ///< Forward gap

  int colletMat;                              ///< Collet material

  std::vector<collInsertBlock> iBlock;   ///< Inner block

  int colletInnerCell;               ///< Inner collet cell
  int colletOuterCell;               ///< Outer Collet cell

  //--------------
  // FUNCTIONS:
  //--------------
  void populate(const Simulation&);
  void createInsert(Simulation&);  
  void createObjects(Simulation&);  
  void createSurfaces();  
  
  void processColletExclude(Simulation&,const int,
			    const size_t,const size_t);
  double processShutterDrop() const;
  void setTwinComp();

 public:

  ZoomShutter(const size_t,const std::string&,const std::string&);
  ZoomShutter(const ZoomShutter&);
  ZoomShutter& operator=(const ZoomShutter&);
  virtual ~ZoomShutter();

  Geometry::Vec3D getExitTrack() const;
  Geometry::Vec3D getExitPoint() const;

  virtual void createAll(Simulation&,const double,
			 const attachSystem::FixedComp*);

};

}

#endif
 
