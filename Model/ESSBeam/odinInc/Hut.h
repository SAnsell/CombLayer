/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Hut.h
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
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  const int hutIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  


  double voidHeight;            ///< void height [top only]
  double voidWidth;             ///< void width [total]
  double voidDepth;             ///< void depth [low only]
  double voidLength;            ///< void length [total]
  double voidNoseLen;           ///< lenght of nose cone [centre]
  double voidNoseWidth;         ///< length a front point

  double feLeftWall;            ///< Left wall
  double feRightWall;           ///< Right wall
  double feRoof;                ///< Roof
  double feFloor;               ///< Token floor depth
  double feNoseFront;           ///< Thickness of front of nose
  double feNoseSide;            ///< Side thickness [not angle correct]
  double feBack;                ///< Last/Rear wall thickness

  double concLeftWall;            ///< Left wall
  double concRightWall;           ///< Right wall
  double concRoof;                ///< Roof
  double concFloor;               ///< Token floor depth
  double concNoseFront;           ///< Thickness of front of nose
  double concNoseSide;            ///< Side thickness [not angle correct]
  double concBack;                ///< Last/Rear wall thickness

  double wallYStep;               ///< Step from flat cut point
  double wallThick;               ///< Thickness 
  
  int feMat;                  ///< Fe layer material 
  int concMat;                ///< Second layer material
  int wallMat;                ///< Second layer material
  
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
 
