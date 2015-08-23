/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/ChopperPit.h
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
#ifndef constructSystem_ChopperPit_h
#define constructSystem_ChopperPit_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class ChopperPit
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief ChopperPit unit  
*/

class ChopperPit :
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

  double feHeight;            ///< fe height [top only]
  double feDepth;             ///< fe depth [low only]
  double feWidth;             ///< fe width [total]
  double feFront;             ///< fe front 
  double feBack;              ///< fe front 

  double concHeight;            ///< concrete height [top only]
  double concDepth;             ///< concrete depth [low only]
  double concWidth;             ///< concrete width [total]
  double concFront;             ///< concrete front 
  double concBack;              ///< concrete back 

  double colletWidth;           ///< Width of collette
  double colletDepth;           ///< Depth of collette
  double colletHeight;          ///< Height of collette
  
  int feMat;                  ///< Fe material layer
  int concMat;                ///< conc material layer
  int colletMat;              ///< Collette material 
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const std::string&);
  void createLinks();

 public:

  ChopperPit(const std::string&);
  ChopperPit(const ChopperPit&);
  ChopperPit& operator=(const ChopperPit&);
  virtual ~ChopperPit();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const std::string&);

};

}

#endif
 
