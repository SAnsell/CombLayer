/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoomInc/ZoomRoof.h
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
#ifndef zoomSystem_ZoomRoof_h
#define zoomSystem_ZoomRoof_h

class Simulation;

namespace zoomSystem
{

/*!
  \class ZoomRoof
  \version 1.0
  \author S. Ansell
  \date September 2011
  \brief ZoomRoof unit
  
*/

class ZoomRoof : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int roofIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< populated or not

  size_t nSteel;                   ///< Number of items
  size_t nExtra;                   ///< Number of items
  std::vector<varBlock> Steel;  ///< List of items
  std::vector<varBlock> Extra;  ///< List of items

  int steelMat;                 ///< Steel material
  int extraMat;                 ///< Extra material

  int monoWallSurf;      ///< Montolith Exit surface
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces(const shutterSystem::GeneralShutter&,
		      const attachSystem::FixedComp&,
		      const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void layerProcess(Simulation&);

 public:

  ZoomRoof(const std::string&);
  ZoomRoof(const ZoomRoof&);
  ZoomRoof& operator=(const ZoomRoof&);
  virtual ~ZoomRoof();

  /// Set surface
  void setMonoSurface(const int M) { monoWallSurf=M; }
  void createAll(Simulation&,const shutterSystem::GeneralShutter&,
		 const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&);
};

}

#endif
 
