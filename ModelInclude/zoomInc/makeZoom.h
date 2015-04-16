/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/makeZoom.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef zoomSystem_makeZoom_h
#define zoomSystem_makeZoom_h

namespace constructSystem
{
  class DiskChopper;
}

namespace zoomSystem
{
  class ZoomBend;
  class ZoomChopper;
  class ZoomCollimator;
  class ZoomOpenStack;
  class ZoomRoof;
  class ZoomPrimary;
  class ZoomHutch;

  /*!
    \class makeZoom 
    \version 1.0
    \author S. Ansell
    \date June 2011
  */

class makeZoom
{
 private:

  std::shared_ptr<ZoomBend> ZBend;              ///< Bend Object
  std::shared_ptr<ZoomChopper> ZChopper;        ///< Chopper room
  ///< Chopper disk
  std::shared_ptr<constructSystem::DiskChopper> ZDisk; 
  std::shared_ptr<ZoomCollimator> ZCollimator;  ///< Collimator object
  std::shared_ptr<ZoomOpenStack> ZColInsert;    ///< collimator insert pieces
  std::shared_ptr<ZoomPrimary> ZPrim;           ///< Zoom Primary
  std::shared_ptr<ZoomHutch> ZHut;              ///< Hut object

 public:
  
  makeZoom();
  makeZoom(const makeZoom&);
  makeZoom& operator=(const makeZoom&);
  ~makeZoom();
  

  /// Bend Object
  const ZoomBend& getBend() const { return *ZBend; }              
  /// Chopper
  const ZoomChopper& getChopper() const { return *ZChopper; }
  /// Collimator
  const ZoomCollimator& getCollimator() const { return *ZCollimator; } 
  /// Primary
  const ZoomPrimary& getPrimary() const { return *ZPrim; }  
  /// Hutch
  const ZoomHutch& getHutch() const { return *ZHut; }

  void buildIsolated(Simulation&);
  void build(Simulation&,const mainSystem::inputParam&,
	     const shutterSystem::BulkShield&);

};

}

#endif
