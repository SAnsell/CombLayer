/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BlockStand.h
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
#ifndef xraySystem_BlockStand_h
#define xraySystem_BlockStand_h

class Simulation;

namespace xraySystem
{
  /*!
    \class BlockStand
    \version 1.0
    \author S. Ansell
    \date June 2018
    \brief Extra beam stop for opticshutch
  */
  
class BlockStand :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:
  
  double height;                 ///< thickness of collimator
  double width;                  ///< main width
  double length;                 ///< thickness of collimator
  
  int mat;                       ///< material
  
  void createUnitVector(const attachSystem::FixedComp&,
			const long int) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:
  
  BlockStand(const std::string&);
  BlockStand(const BlockStand&);
  BlockStand& operator=(const BlockStand&);
  ~BlockStand() override {}  ///< Destructor

  void populate(const FuncDataBase&) override;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
