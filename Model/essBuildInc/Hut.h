/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Hut.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_Hut_h
#define essSystem_Hut_h

class Simulation;

namespace essSystem
{
  
/*!
  \class Hut
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief Hut unit  
*/

class Hut :
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
  
{
 private:
  
  const int pitIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  


  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]
  double voidNoseLen;           ///< lenght of nose cone [centre]
  double voidNoseWidth;         ///< void length [total]

  double feLeftWall;
  double feRightWall;
  double feRoof;
  double feFloor;
  double feNoseWall;
  double feNoseSide;
  double feBack;

  int feMat;                  //< Fe material layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Hut(const std::string&);
  Hut(const Hut&);
  Hut& operator=(const Hut&);
  virtual ~Hut();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
