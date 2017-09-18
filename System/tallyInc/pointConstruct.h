/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/pointConstruct.h
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
#ifndef tallySystem_pointConstruct_h
#define tallySystem_pointConstruct_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}


class Simulation;

namespace tallySystem
{

/*!
  \class pointConstruct
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief Holds everthing for tallies

  Provides linkage to its outside on FixedComp[0]
*/

class pointConstruct
{
 public:

  pointConstruct();
  pointConstruct(const pointConstruct&);
  pointConstruct& operator=(const pointConstruct&);
  virtual ~pointConstruct() {}  ///< Destructor
  
  // Point Stuff
  virtual void processPoint(Simulation&,const mainSystem::inputParam&,
		    const size_t) const;
  void processPointFree(Simulation&,const Geometry::Vec3D&,
			const std::vector<Geometry::Vec3D>&) const;
  void processPointFree(Simulation&,const std::string&,
			  const long int,const double) const;
  void processPointFree(Simulation&,const std::string&,
			const long int,const Geometry::Vec3D&) const;
  void processPointWindow(Simulation&,const std::string&,
			  long int,const double,
			  const double,const double) const;


  virtual void writeHelp(std::ostream&) const;

  static std::vector<Geometry::Vec3D> 
    calcWindowIntercept(const int,const std::vector<int>,
			const Geometry::Vec3D&);
};

}

#endif
 
