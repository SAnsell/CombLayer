/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/meshConstruct.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef tallySystem_meshConstruct_h
#define tallySystem_meshConstruct_h

namespace attachSystem
{
  class FixedComp;
}

class Simulation;

namespace tallySystem
{

/*!
  \class meshConstruct
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief Constructs a mesh tally from inputParam
*/

class meshConstruct : virtual public basicConstruct
{
 private:
  
  static const std::string& getDoseConversion();

  void rectangleMesh(Simulation&,const int,const std::string&,
		     const Geometry::Vec3D&,const Geometry::Vec3D&,
		     const int*) const;



  
 public:

  meshConstruct();
  meshConstruct(const meshConstruct&);
  meshConstruct& operator=(const meshConstruct&);
  ~meshConstruct() {}  ///< Destructor

  // Point Stuff
  void processMesh(Simulation&,const mainSystem::inputParam&,
		   const size_t) const;
  
};

}

#endif
 
