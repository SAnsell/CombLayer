/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BremBlock.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef xraySystem_BremBlock_h
#define xraySystem_BremBlock_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class BremBlock
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief Bremsstralung Collimator unit  
*/

class BremBlock :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  double radius;              ///< Main radius
  double length;              ///< Main length  

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double holeAWidth;           ///< Front width of hole
  double holeAHeight;          ///< Front height of hole
  double holeBWidth;           ///< Back width of hole
  double holeBHeight;          ///< Back height of hole
  
  int voidMat;                ///< void material
  int mainMat;                ///< main material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  BremBlock(const std::string&);
  BremBlock(const BremBlock&);
  BremBlock& operator=(const BremBlock&);
  virtual ~BremBlock();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
