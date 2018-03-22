/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/FrontEndCave.h
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
#ifndef xraySystem_Wiggler_h
#define xraySystem_Wiggle_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class Wiggler
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Wiggler magnetic chicane

  Built around the central beam axis
*/

class Wiggler :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
  
{
 private:
  
  const int wigIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double length;                  ///< Main length
  double blockWidth;              ///< Block [quad unit] width
  double blockDepth;              ///< Depth of unit
  double blockHGap;               ///< mid horrizontal gap
  double blockVGap;               ///< vertical gap
  double blockHCorner;            ///< horrizontal corner cut
  double blockVCorner;            ///< vertical corner cut

  int voidMat;                    ///< Void material
  int blockMat;                   ///< Block material
  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Wiggler(const std::string&);
  Wiggler(const Wiggler&);
  Wiggler& operator=(const Wiggler&);
  virtual ~Wiggler();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
