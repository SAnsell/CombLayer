/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuildInc/collInsert.h
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
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffsetGroup
{
 protected:

  const std::string baseName;   ///< basename
  const int blockID;            ///< Block number

  double hGap;          ///< B4C horizontal gap 
  double vGap;          ///< B4C vertical 
  
  double length;        ///< length of unit 
  double width;         ///< Full width
  double height;        ///< Full height

  int mat;             ///< Material number  

  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  
  virtual void populate(const FuncDataBase&);
  virtual void createSurfaces();
  virtual void createObjects(Simulation&);
  void createLinks();
  
 public:

  collInsert(const std::string&,const int);
  collInsert(const collInsert&);
  collInsert& operator=(const collInsert&);
  virtual ~collInsert() {}   ///< Destructor

  Geometry::Vec3D getWindowCentre() const;

  void createAll(Simulation&,const FixedComp&,const long int);
};

std::ostream& operator<<(std::ostream&,const collInsert&);
}  

#endif
