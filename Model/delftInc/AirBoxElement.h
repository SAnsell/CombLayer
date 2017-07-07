/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/AirBoxElement.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef delftSystem_AirBoxElement_h
#define delftSystem_AirBoxElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class AirBoxElement
  \version 1.0
  \author S. Ansell/A. Winklman 
  \date October 2013
  \brief Creates an airbox element
*/

class AirBoxElement  : public RElement
{
 private:

  double Width;           ///< Width of outer
  double Depth;           ///< Depth of outer
  double Height;          ///< Height of outer

  double wallThick;       ///< Wall thickness
  double edgeGap;        ///< loc thickness

  int innerMat;           ///< inner material
  int wallMat;            ///< Wall material 
  int waterMat;           ///< Water coolant 

  void populate(const FuncDataBase&);
  void populateWaterMat(const attachSystem::FixedComp&);
  void createUnitVector(const attachSystem::FixedComp&,
			const Geometry::Vec3D&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  AirBoxElement(const size_t,const size_t,const std::string&);
  AirBoxElement(const AirBoxElement&);
  AirBoxElement& operator=(const AirBoxElement&);
  virtual ~AirBoxElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const Geometry::Vec3D&,const FuelLoad&);

};

}  

#endif
