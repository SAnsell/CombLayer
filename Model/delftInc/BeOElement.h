/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/BeOElement.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef delftSystem_BeOElement_h
#define delftSystem_BeOElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class BeOElement
  \version 1.0
  \author S. Ansell
  \date April 2014
  \brief Creates an irradiation element
*/

class BeOElement  : public RElement
{
 private:

  double Width;           ///< Width of outer
  double Depth;           ///< Depth of outer
  double TopHeight;       ///< Top height (from origin)

  double wallThick;       ///< Wall thickness [outer]
  double coolThick;       ///< Coolant thickness 

  int beMat;              ///< Default [be material]
  int coolMat;              ///< Default [be material]
  int wallMat;              ///< Default [be material]

  void populate(const Simulation&);
  void createUnitVector(const FixedComp&,const Geometry::Vec3D&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeOElement(const size_t,const size_t,const std::string&);
  BeOElement(const BeOElement&);
  BeOElement& operator=(const BeOElement&);
  virtual ~BeOElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const Geometry::Vec3D&,
			 const FuelLoad&);

};

}  

#endif
