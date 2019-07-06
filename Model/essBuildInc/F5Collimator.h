/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/F5Collimator.h
 *
 * Copyright (c) 2015-2019 Konstantin Batkov
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
#ifndef essSystem_F5Collimator_h
#define essSystem_F5Collimator_h

class Simulation;

namespace essSystem
{

/*!
  \class F5Collimator
  \version 1.0
  \author K. Batkov. 
  \date May 2015
  \brief F5 collimator to restrict angular flux direction
*/

class F5Collimator : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public tallySystem::F5Calc
{
 private:

  double height;                  ///< Height along z
  double length;                   ///< Width  along x
  double width;                   ///< Width  along y
  double wall;                    ///< Wall thickness
  
  int GluePoint;              ///< Glue point number (currently number defines quadrant as in butterfly.svgz). Not used if <0
  
  // Functions:
  
  void populate(FuncDataBase&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
  
 public:

  F5Collimator(const std::string&);
  F5Collimator(const F5Collimator&);    
  F5Collimator& operator=(const F5Collimator&);
  virtual ~F5Collimator();
  
  int getMainCell() const { return buildIndex+1; }
  virtual void addToInsertChain(attachSystem::ContainedComp&) const; 

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
