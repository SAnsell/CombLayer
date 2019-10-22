/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModelInc/AreaDetector.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef instrumentSystem_AreaDetector_h
#define instrumentSystem_AreaDetector_h

class Simulation;


namespace saxsSystem
{

/*!
  \class AreaPlate
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Grid of detector objects 
*/

class AreaPlate : public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap
{
 private:

  size_t NWBin;             ///< Width bins
  size_t NHBin;             ///< Horrizontal bins
  double width;             ///< width
  double height;            ///< Height
  double thick;             ///< thickness
  
  int mainMat;                  ///< Wall material

  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  AreaPlate(const std::string&);
  AreaPlate(const AreaPlate&);
  AreaPlate& operator=(const AreaPlate&);
  virtual ~AreaPlate();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
