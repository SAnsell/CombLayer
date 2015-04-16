/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/DefElement.h
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
#ifndef delftSystem_DefElement_h
#define delftSystem_DefElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class DefElement
  \version 1.0
  \author S. Ansell
  \date September 2010
  \brief Blocks in shutter
  
  Holds the edge/centre 
*/

class DefElement  : public RElement
{
 private:

  void populate(const FuncDataBase&);
  void createUnitVector(const FixedComp&,const Geometry::Vec3D&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DefElement(const size_t,const size_t,const std::string&);
  DefElement(const DefElement&);
  DefElement& operator=(const DefElement&);
  virtual ~DefElement() {}   ///< Destructor

  void createAll(Simulation&,const FixedComp&,
		 const Geometry::Vec3D&,const FuelLoad&);

};

}  

#endif
