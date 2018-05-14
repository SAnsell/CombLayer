/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/BeElement.h
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
#ifndef delftSystem_BeElement_h
#define delftSystem_BeElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class BeElement
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Creates an irradiation element
*/

class BeElement  : public RElement
{
 private:

  double Width;           ///< Width of outer
  double Depth;           ///< Depth of outer
  double TopHeight;       ///< Top height (from origin)

  size_t nLayer;          ///< number of layers
  int beMat;              ///< Default be material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const Geometry::Vec3D&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&,const FuelLoad&); 

 public:

  BeElement(const size_t,const size_t,const std::string&);
  BeElement(const BeElement&);
  BeElement& operator=(const BeElement&);
  virtual ~BeElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const Geometry::Vec3D&,
			 const FuelLoad&);

};

}  

#endif
