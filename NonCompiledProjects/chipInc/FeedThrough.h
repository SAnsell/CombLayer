/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/FeedThrough.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef hutchSystem_FeedThrough_h
#define hutchSystem_FeedThrough_h

class Simulation;

namespace hutchSystem
{
  class chipIRHutch;

/*!
  \class FeedThrough
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief FeedThrough [insert object]
*/

class FeedThrough : public attachSystem::FixedUnit
{
 private:

  const std::string baseName;   ///< Base name
  const size_t ID;              ///< Identity number
  
  ModelSupport::BoxLine CollTrack;   ///< Collimator track
  
  double height;                  ///< Height of the feed through
  double width;                   ///< Width of the basic feed through
  Geometry::Vec3D Offset;         ///< 
  std::vector<Geometry::Vec3D> CPts;

  
  void populate(const FuncDataBase&);
  void insertColl(Simulation&,const chipIRHutch&);

 public:

  FeedThrough(const std::string&,const size_t);
  FeedThrough(const FeedThrough&);
  FeedThrough& operator=(const FeedThrough&);
  ~FeedThrough();
  
  void build(Simulation&,const chipIRHutch&);

};

}

#endif
 
