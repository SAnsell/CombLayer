/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/DHut.h
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
#ifndef essSystem_DHut_h
#define essSystem_DHut_h

class Simulation;

namespace essSystem
{
  
/*!
  \class DHut
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief DHut unit  
*/

class DHut :
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
  double voidFrontCut;          ///< void cut on front
  double voidFrontStep;         ///< void length down (Y)
  double voidBackCut;           ///< void cut on X
  double voidBackStep;          ///< void cut on Y

  double feThick;               ///< Fe Thickness
  double concThick;             ///< Wall thickness
  
  int feMat;                  ///< Fe layer material 
  int concMat;                ///< Second layer material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DHut(const std::string&);
  DHut(const DHut&);
  DHut& operator=(const DHut&);
  virtual ~DHut();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
