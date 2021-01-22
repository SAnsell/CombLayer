/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/FMUndulator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef xraySystem_FMUndulator_h
#define xraySystem_FMUndulator_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class FMUndulator
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief FMUndulator magnetic chicane

  Built around the central beam axis
*/

class FMUndulator :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:

  double vGap;                     ///< Vertical gap
 
  double length;                   ///< Main length
  double poleWidth;                ///< Block [close] width
  double poleDepth;                ///< Depth of unit

  double magnetWidth;              ///< Block [main] width
  double magnetDepth;              ///< Depth of unit

  double baseDepth;                ///< support thick [z axis]
  double baseExtraLen;             ///< support extra [y axis + length]

  double midWidth;                 ///< Width of mid support 
  double midDepth;                 ///< Depth of mid support
  
  double mainWidth;               ///< stand depth [x]
  double mainDepth;               ///< stand depth [z]

  int voidMat;                     ///< Void material
  int magnetMat;                   ///< Block material
  int supportMat;                  ///< support material
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FMUndulator(const std::string&);
  FMUndulator(const FMUndulator&);
  FMUndulator& operator=(const FMUndulator&);
  virtual ~FMUndulator();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
