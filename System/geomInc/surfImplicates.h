/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geomInc/surfImplicates.h
*
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef surfImplicates_h
#define surfImplicates_h

namespace Geometry
{
  class Surface;
}

namespace Geometry
{

/*!
  \class surfImplicates 
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief Process surface implicates
*/

class surfImplicates
{
 public:

  /// surface implicate system
  typedef std::pair<int,int> (surfImplicates::*testImp)
    (const Surface*,const Surface*) const;
  
  /// storage of tracking map
  typedef std::map<std::string,testImp> STYPE;  
  
 private:
 
  STYPE functionMap;      ///< surfname : testImp function
  
  surfImplicates();

  ////\cond SINGLETON
  surfImplicates(const surfImplicates&);
  surfImplicates& operator=(const surfImplicates&);
  ////\endcond SINGLETON

  std::pair<int,int> planePlane(const Geometry::Surface*,
				const Geometry::Surface*) const;
  std::pair<int,int> planeCylinder(const Geometry::Surface*,
				   const Geometry::Surface*) const;
  std::pair<int,int> cylinderPlane(const Geometry::Surface*,
				   const Geometry::Surface*) const;
  
  
 public:
  
  static surfImplicates& Instance();

  /// destructor
  ~surfImplicates() {}
 
  std::pair<int,int> isImplicate(const Geometry::Surface*,
				 const Geometry::Surface*) const;
};

}

#endif
