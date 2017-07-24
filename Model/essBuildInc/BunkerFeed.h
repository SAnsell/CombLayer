/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerFeed.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_BunkerFeed_h
#define essSystem_BunkerFeed_h

class Simulation;

namespace ModelSupport
{
  class BoxLine;
}

namespace essSystem
{

/*!
  \class BunkerFeed
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BunkerFeed [insert object]
*/

class BunkerFeed :
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:
  
  const size_t ID;              ///< Identity number
  const std::string baseName;   ///< Base name
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  ModelSupport::BoxLine* voidTrack;   ///< Collimator track
  
  double height;                  ///< Height of the feed through
  double width;                   ///< Width of the feed through
  Geometry::Vec3D Offset;         ///< Primary offset position

  std::vector<Geometry::Vec3D> CPts;  ///< Points relative to origin

  void populate(const FuncDataBase&);
  void createUnitVector(const Bunker&,const size_t);
  void insertColl(Simulation&);
  void moveToLayer(const Bunker&,const std::string&);
  
 public:

  BunkerFeed(const std::string&,const size_t);
  BunkerFeed(const BunkerFeed&);
  BunkerFeed& operator=(const BunkerFeed&);
  ~BunkerFeed();
  
  void createAll(Simulation&,const Bunker&,
		 const size_t,const std::string&);

};

}

#endif
 
