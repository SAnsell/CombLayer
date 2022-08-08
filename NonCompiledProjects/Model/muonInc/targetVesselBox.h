/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muonInc/targetVesselBox.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Goran Skoro
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
#ifndef muSystem_targetVesselBox_h
#define muSystem_targetVesselBox_h

class Simulation;

namespace muSystem
{

/*!
  \class targetVesselBox
  \author G. Skoro
  \version 1.0
  \date October 2013
  \brief mu target vessel box
*/

class targetVesselBox :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp
{
 private:
  
  double height;                ///< Height
  double depth;                 ///< Depth
  double width;                 ///< Width

  double steelThick;            ///< Steel thickness

  int steelMat;                 ///< Material: Steel


  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  targetVesselBox(const std::string&);
  targetVesselBox(const targetVesselBox&);
  targetVesselBox& operator=(const targetVesselBox&);
  virtual ~targetVesselBox();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);  
};

}

#endif
 
