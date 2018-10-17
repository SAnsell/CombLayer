/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/SimpleChicane.h
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
#ifndef xraySystem_SimpleChicane_h
#define xraySystem_SimpleChicane_h

class Simulation;

namespace xraySystem
{
  /*!
    \class SimpleChicane
    \version 1.0
    \author S. Ansell
    \date June 2018
    \brief Simple chicane with no inner plate
  */
  
class SimpleChicane : public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double height;                 ///< height of system 
  double width;                  ///< widht of syste
  double clearGap;               ///< clearance between plates
  double upStep;                 ///< distance wall falls into gap

  double plateThick;              ///< skin thickness

  double innerXWidth;           ///< Inner cross section
  double innerXDepth;            ///< Inner cross section 
    
  int plateMat;                   ///< plate material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  
 public:
  
  SimpleChicane(const std::string&);
  SimpleChicane(const SimpleChicane&);
  SimpleChicane& operator=(const SimpleChicane&);
  virtual ~SimpleChicane() {}  ///< Destructor


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
