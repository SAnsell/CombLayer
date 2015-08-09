/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/basicConstruct.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef tallySystem_basicConstruct_h
#define tallySystem_basicConstruct_h

class Simulation;

namespace mainSystem
{
  class inputParam;
}
namespace tallySystem
{

/*!
  \class basicConstruct
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief Holds everthing for tallies

  Provides linkage to its outside on FixedComp[0]
*/

class basicConstruct 
{
 protected:

  
  int convertRegion(const mainSystem::inputParam&,
		    const std::string&,const size_t,
		    const size_t,int&,int&) const;

  template<typename T>
  T inputItem(const mainSystem::inputParam&,
	      const size_t,const size_t,
	      const std::string&) const;

  
  Geometry::Vec3D
    inputCntVec3D(const mainSystem::inputParam&,
		  const size_t,size_t&,
		  const std::string&) const;

 
  template<typename T>
  int checkItem(const mainSystem::inputParam&,
		const size_t,const size_t,T&) const;

  long int getLinkIndex(const std::string&) const;
  long int  getLinkIndex(const mainSystem::inputParam&,const size_t,
		   const size_t) const;

  std::vector<int> getCellSelection(const Simulation&,
				    const int,const int,
				    const int) const;

 public:

  basicConstruct();
  basicConstruct(const basicConstruct&);
  basicConstruct& operator=(const basicConstruct&);
  virtual ~basicConstruct() {}  ///< Destructor

  virtual void writeHelp(std::ostream&) const =0;
  
  static int convertRange(const std::string&,int&,int&);
  
};

}

#endif
 
