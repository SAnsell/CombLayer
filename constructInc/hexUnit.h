/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   constructInc/hexUnit.h
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
#ifndef constructSystem_hexUnit_h
#define constructSystem_hexUnit_h

namespace constructSystem
{

/*!
  \class hexUnit
  \version 1.0
  \author S. Ansell
  \date May 2013
  \brief Deals with a number of tubes in a plate
 */

class hexUnit
{
 private:  

  int iA;                           ///< Index A
  int iB;                           ///< Index B
  Geometry::Vec3D Centre;           ///< Centre 

  std::vector<hexUnit*> hexLink;         ///< Surf keys
  std::vector<int> surfKey;         ///< Surf keys
  int cellNumber;                   ///< Designated cell number


 public:

  static int hexIndex(const size_t);

  hexUnit(const int,const int,const Geometry::Vec3D&);
  hexUnit(const hexUnit&);
  hexUnit& operator=(const hexUnit&); 
  ~hexUnit() {}  ///< Destructor

  int getIndex() const { return 1000*iA+iB; }
  bool isComplete() const;
  bool isConnected(const hexUnit&) const;
  size_t nLinks() const;
  /// Access centre
  const Geometry::Vec3D& getCentre() const { return Centre; }
  void setSurf(const size_t,const int); 
  
  int createLinkSurf();
  bool hasLink(const size_t) const;

  std::string getShell() const;
  std::string getInner() const;

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const hexUnit&);

}

#endif
 
