/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/LinkedComp.h
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
#ifndef LinkedComp_h
#define LinkedComp_h

class Rule;

namespace attachSystem
{
/*!
  \class LinkedComp
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief Allows an object to be connected 

  Store a number of link points to an object, e.g. Reflector/Moderator
*/

class LinkedComp  
{
 private:

  std::vector<LinkUnit> LU;        ///< Set of objects
  
 protected:
  
  void setAxis(const int,const Geometry::Vec3D&,
	       const Geometry::Vec3D&,
	       const Geometry::Vec3D&);

  void setCentre(const int,const Geometry::Vec3D&);
  void setExtent(const int,const double,const double,const double);

  void addLinkSurf(const int,const int);
  void addLinkSurf(const int,const std::string&);

 public:

  explicit LinkedComp(const int);
  LinkedComp(const LinkedComp&);
  LinkedComp& operator=(const LinkedComp&);
  virtual ~LinkedComp();

  /// How many connections
  int getSize() const { return static_cast<int>(LU.size()); }
  const LinkUnit& getLU(const int)  const; 

  int getLinkSurf(const int) const;
  std::string getLinkString(const int) const;

};

}

#endif
 
