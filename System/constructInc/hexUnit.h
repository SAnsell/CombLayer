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

  bool empty;                       ///< Flag to turn cell off
  bool cut;                         ///< Flag if cell cut by outer boundary
  int iA;                           ///< Index A
  int iB;                           ///< Index B
  Geometry::Vec3D Centre;           ///< Centre 

  std::vector<hexUnit*> hexLink;    ///< Links
  std::vector<int> cylSurf;         ///< Cylinder numbers
  std::vector<int> surfKey;         ///< Surf keys
  int cellNumber;                   ///< Designated cell number
  
  std::string cutStr;               ///< External cutting surf
  
 public:

  static int hexIndex(const size_t);

  hexUnit(const int,const int,const Geometry::Vec3D&);  
  hexUnit(const int,const int,const bool,const Geometry::Vec3D&);
  hexUnit(const hexUnit&);
  hexUnit& operator=(const hexUnit&); 
  ~hexUnit() {}  ///< Destructor

  /// Index value 
  int getIndex() const { return 1000*iA+iB; }
  /// Void cell
  bool isEmpty() const { return empty; }
  bool isComplete() const;
  bool isCut() const { return cut; }
  bool isConnected(const hexUnit&) const;
  const std::string& getCut() const { return cutStr; }
  size_t nLinks() const;
  /// Access centre
  const Geometry::Vec3D& getCentre() const { return Centre; }
  void setSurf(const size_t,const int); 
  /// Simple setter
  void setCutString(const std::string& S) { cutStr=S; }
  void clearLinks();

  int createLinkSurf();
  bool hasLink(const size_t) const;

  void setCyl(const int);
  void addCyl(const int);

  std::string getShell() const;
  std::string getInner() const;

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const hexUnit&);

}

#endif
 
