/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuildInc/collInsert.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef shutterSystem_collInsert_h
#define shutterSystem_collInsert_h

class FuncDataBase;

namespace shutterSystem
{

/*!
  \class collInsert
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class collInsert  :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut
{
 private:

  double fStep;          ///< forward step

  double length;        ///< length of unit 
  double width;         ///< Full width
  double height;        ///< Full height

  double hGap;          ///< B4C horizontal gap 
  double vGap;          ///< B4C vertical 

  double hGapRAngle;          ///< B4C horizontal gap angle change
  double vGapRAngle;          ///< B4C vertical gap angle change

  size_t nB4C;                 ///< number of B4C units
  size_t nSteel;               ///< number of steel blocks

  double steelOffset;          ///< Steel back step distance
  double steelAWidth;          ///< Steel width front half units
  double steelBWidth;          ///< Steel width back half units
  double b4cThick;             ///< b4c layers
  double b4cGap;

  int mat;             ///< Material number  


  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  collInsert(const std::string&);
  collInsert(const collInsert&);
  collInsert& operator=(const collInsert&);
  virtual ~collInsert() {}   ///< Destructor

  void populate(const FuncDataBase&);
  
  double calcDrop(const double) const;
  Geometry::Vec3D getWindowCentre() const;

  using FixedComp::createAll;
  void createAll(Simulation&,const FixedComp&,const long int);
};


}  

#endif
