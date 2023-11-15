/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1Ring.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_M1Ring_h
#define xraySystem_M1Ring_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1Ring
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1Ring :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  

  double outerThick;          ///< thickness from outer surf (<inner)
  double outerLength;
  double innerYStep;          ///< front back shift
  double innerThick;          ///< thickness from outer surf
  double innerLength;         ///< inner lenfth 
  
  int ringMat;            ///< Electron shield material
  int voidMat;                ///< void material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1Ring(const std::string&);
  M1Ring(const M1Ring&);
  M1Ring& operator=(const M1Ring&);
  ~M1Ring() override;
  void adjustExtraVoids(Simulation&,const int,const int,const int);
  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
