/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BifrostHut.h
 *
 * Copyright (c) 2004-2017- by Stuart Ansell
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
#ifndef essSystem_BifrostHut_h
#define essSystem_BifrostHut_h

class Simulation;

namespace essSystem
{
  
/*!
  \class BifrostHut
  \version 1.0
  \author S. Ansell
  \date July 2016
  \brief BifrostHut unit  
*/

class BifrostHut :
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
  
{
 private:
  
  const int hutIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  


  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]

  double feFront;               ///< Front extension
  double feLeftWall;            ///< Left wall
  double feRightWall;           ///< Right wall
  double feRoof;                ///< Roof
  double feFloor;               ///< Token floor depth
  double feBack;                ///< Back steel extension

  double concFront;               ///< Front extension
  double concLeftWall;            ///< Left wall
  double concRightWall;           ///< Right wall
  double concRoof;                ///< Roof
  double concFloor;               ///< Token floor depth
  double concBack;                ///< Back length
  
  int feMat;                  ///< Fe layer material 
  int concMat;                ///< Second layer material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BifrostHut(const std::string&);
  BifrostHut(const BifrostHut&);
  BifrostHut& operator=(const BifrostHut&);
  virtual ~BifrostHut();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
