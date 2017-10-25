/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/gridConstruct.h
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
#ifndef tallySystem_gridConstruct_h
#define tallySystem_gridConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace tallySystem
{

/*!
  \class gridConstruct
  \version 1.0
  \author S. Ansell
  \date Januray 2015
  \brief Construct grid tallies

*/

class gridConstruct 
{
 private:

  void applyMultiGrid(Simulation&,const size_t,const size_t,
		      const Geometry::Vec3D&,
		      const Geometry::Vec3D&,
		      const Geometry::Vec3D&) const;

  int calcGlobalCXY(const std::string&,
		    const long int,Geometry::Vec3D&,
		    Geometry::Vec3D&,Geometry::Vec3D&) const;
  
 public:

  gridConstruct();
  gridConstruct(const gridConstruct&);
  gridConstruct& operator=(const gridConstruct&);
  virtual ~gridConstruct() {}  ///< Destructor
  
  // Point Stuff
  virtual void processGrid(Simulation&,const mainSystem::inputParam&,
		    const size_t) const;


  virtual void writeHelp(std::ostream&) const;

};

}

#endif
 
