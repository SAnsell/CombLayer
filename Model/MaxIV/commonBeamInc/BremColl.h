/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BremColl.h
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
#ifndef xraySystem_BremColl_h
#define xraySystem_BremColl_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class BremColl
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief Bremsstralung Collimator unit  
*/

class BremColl :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  double height;              ///< Height of W block
  double width;               ///< Width of W block
  double length;              ///< Main length  
  double wallThick;           ///< wall thickness of main tube

  double innerRadius;          ///< Inner radius of hole
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double holeAWidth;           ///< Front width of hole
  double holeAHeight;          ///< Front height of hole
  double holeBWidth;           ///< Back width of hole
  double holeBHeight;          ///< Back height of hole
  
  int voidMat;                ///< void material
  int innerMat;               ///< void material
  int wallMat;                ///< Fe material layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  BremColl(const std::string&);
  BremColl(const BremColl&);
  BremColl& operator=(const BremColl&);
  virtual ~BremColl();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
