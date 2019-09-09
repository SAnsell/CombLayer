/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/PortChicane.h
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
#ifndef xraySystem_PortChicane_h
#define xraySystem_PortChicane_h

class Simulation;

namespace xraySystem
{
  /*!
    \class PortChicane
    \version 1.0
    \author S. Ansell
    \date June 2018
    \brief Chicanes in the optics/expt hutches
  */
  
class PortChicane : public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double height;                 ///< height of system 
  double width;                  ///< widht of syste
  double clearGap;               ///< clearance between plates
  double downStep;               ///< distance wall falls into gap
  double overHang;               ///< extra round plates

  double innerSkin;              ///< skin thickness
  double innerPlate;             ///< plate thickness

  double outerSkin;              ///< skin thickness
  double outerPlate;             ///< plate thickness

  double baseThick;              ///< thickness of base
  double wallThick;              ///< thickness of base

  bool frontRemove;              ///< Removed front plate
  bool backRemove;               ///< Removed back plate
  
  int plateMat;                   ///< plate material
  int skinMat;                    ///< plate skin material
  int wallMat;                    ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:
  
  PortChicane(const std::string&);
  PortChicane(const PortChicane&);
  PortChicane& operator=(const PortChicane&);
  virtual ~PortChicane() {}  ///< Destructor


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
