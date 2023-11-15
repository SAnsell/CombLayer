/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   softimaxInc/M1ElectronShield.h
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
#ifndef xraySystem_M1ElectronShield_h
#define xraySystem_M1ElectronShield_h

class Simulation;

namespace xraySystem
{

/*!
  \class M1ElectronShield
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focusable mirror in mount
*/

class M1ElectronShield :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  // electron shield
  double elecXOut;            ///< xout step distance
  double elecLength;          ///< length
  double elecThick;           ///< thickness
  double elecHeight;          ///< height
  double elecEdge;            ///< edge thickness
  double elecHoleRadius;      ///< central hole radius

  
  int electronMat;            ///< Electron shield material
  int voidMat;                ///< void material

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  M1ElectronShield(const std::string&);
  M1ElectronShield(const M1ElectronShield&);
  M1ElectronShield& operator=(const M1ElectronShield&);
  ~M1ElectronShield() override;
  void adjustExtraVoids(Simulation&,const int,const int,const int);
  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
