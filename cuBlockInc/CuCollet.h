/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   cuBlockInc/CuCollet.h
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
#ifndef CuSystem_CuCollet_h
#define CuSystem_CuCollet_h

class Simulation;

namespace cuSystem
{

/*!
  \class CuCollet
  \author S. Ansell
  \version 1.0
  \date December 2012
  \brief Simple Cu block object
*/

class CuCollet : public attachSystem::FixedComp,
    public attachSystem::ContainedComp
{
 private:
  
  const int cuIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                 ///< X-Step
  double yStep;                 ///< Y-Step
  double zStep;                 ///< Z-Step

  double radius;                ///< Cu size
  double cuRadius;              ///< Cu outer size
  double holeRadius;            ///< Hole size
  double cuGap;                 ///< Separation of the Cu

  double cerThick;              ///< Ceramic thickness
  double steelThick;            ///< Steel thickness
  double cuThick;              ///< Copper thickness

  int cerMat;                  ///< Ceramic material
  int steelMat;                ///< Steel material 
  int cuMat;                   ///< Copper material

  void populate(const Simulation&);
  void createUnitVector();

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CuCollet(const std::string&);
  CuCollet(const CuCollet&);
  CuCollet& operator=(const CuCollet&);
  virtual ~CuCollet();

  void createAll(Simulation&);  
};

}

#endif
 
