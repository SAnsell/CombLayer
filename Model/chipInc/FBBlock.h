/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/FBBlock.h
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
#ifndef hutchSystem_FBBlock_h
#define hutchSystem_FBBlock_h

class Simulation;

namespace hutchSystem
{

  class chipIRHutch;

/*!
  \class FBBlock
  \version 1.0
  \author S. Ansell
  \date January 2012
  \brief Feed through block at the chipIR door
*/

class FBBlock : public attachSystem::FixedUnit
{
 private:

  size_t nFeed;           ///< Number of feed throughs

  double fbLength;                      ///< Length of block
  Geometry::Vec3D Offset;               ///< Offset value
  std::vector<Geometry::Vec3D> FBcent;  ///< Centre points
  std::vector<double> FBsize;           ///< Feedblock size

  void populate(const FuncDataBase&);

  void insertBlock(Simulation&,const chipIRHutch&);

 public:

  FBBlock(const std::string&);
  FBBlock(const FBBlock&);
  FBBlock& operator=(const FBBlock&);
  ~FBBlock();
  
  void build(Simulation&,const chipIRHutch&);

};

}

#endif
 
