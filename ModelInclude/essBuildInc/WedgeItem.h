/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/WedgeItem.h
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
#ifndef essSystem_WedgeItem_h
#define essSystem_WedgeItem_h

class Simulation;

namespace essSystem
{

/*!
  \class WedgeItem
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Wedge Item  [single shutter]
*/

class WedgeItem : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int beamNumber;           ///< number of the beamline
  const int wedgeIndex;           ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  size_t nLayer;
  std::vector<double> radius;    ///< Radial cuts [nlayer-1]
  std::vector<double> width;     ///< width of external
  std::vector<double> height;    ///< height of external

  int mat;                     ///< reflector material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const FixedComp&,const size_t,const size_t);
  void createLinks();

 public:

  WedgeItem(const int,const std::string&);
  WedgeItem(const WedgeItem&);
  WedgeItem& operator=(const WedgeItem&);
  virtual ~WedgeItem();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const size_t);
  
};

}

#endif
 
