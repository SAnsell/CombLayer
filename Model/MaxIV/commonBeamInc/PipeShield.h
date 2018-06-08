/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PipeShield.h
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
#ifndef xraySystem_PipeShield_h
#define xraySystem_PipeShield_h

class Simulation;

namespace xraySystem
{
  /*!
    \class PipeShield
    \version 1.0
    \author S. Ansell
    \date June 2018
    \brief Extra beam stop for opticshutch
  */
  
class PipeShield : public attachSystem::ContainedSpace,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double height;                 ///< thickness of collimator
  double width;                  ///< main width
  double length;                 ///< thickness of collimator
  double clearGap;               ///< clearance gap
  double wallThick;              ///< wall thickness
  int mat;                       ///< material
  int wallMat;                   ///< wall material
  
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:
  
  PipeShield(const std::string&);
  PipeShield(const PipeShield&);
  PipeShield& operator=(const PipeShield&);
  virtual ~PipeShield() {}  ///< Destructor

  void populate(const FuncDataBase&);

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
