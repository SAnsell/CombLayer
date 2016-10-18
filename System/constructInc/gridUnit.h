/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/gridUnit.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef constructSystem_gridUnit_h
#define constructSystem_gridUnit_h

namespace constructSystem
{

/*!
  \class gridUnit
  \version 1.0
  \author S. Ansell
  \date October 2016
  \brief Controls connected units in a plate [virtual]

  Virtual top header of units in a plate
  Specialized to hex/rectangle etc
 */

class gridUnit
{
 protected:  

  bool empty;                       ///< Flag to turn cell off
  bool cut;                         ///< Flag if cell cut by outer boundary
  int iA;                           ///< Index A
  int iB;                           ///< Index B
  Geometry::Vec3D Centre;           ///< Centre 

  std::vector<gridUnit*> gridLink;  ///< Links
  std::vector<int> cylSurf;         ///< Cylinder numbers
  std::vector<int> surfKey;         ///< Surf keys
  int cellNumber;                   ///< Designated cell number
  
  std::string cutStr;               ///< External cutting surf
  
 public:

  gridUnit(const size_t,const int,const int,const Geometry::Vec3D&);  
  gridUnit(const size_t,const int,const int,const bool,const Geometry::Vec3D&);
  gridUnit(const gridUnit&);
  gridUnit& operator=(const gridUnit&); 
  virtual ~gridUnit() {}  ///< Destructor

  /// accessor to iA
  int getAIndex() const { return iA; }
  /// accessor to iB
  int getBIndex() const { return iB; }
  /// Index value 
  int getIndex() const { return 1000*iA+iB; }
  /// Void cell
  bool isEmpty() const { return empty; }
  /// access cutter flag
  bool isCut() const { return cut; }
  /// access cutter string
  const std::string& getCut() const { return cutStr; }
  /// Access centre
  const Geometry::Vec3D& getCentre() const { return Centre; }
  /// Simple setter
  void setCutString(const std::string& S) { cutStr=S; }
  void setSurf(const size_t,const int); 
  void clearLinks();
  bool isComplete() const;
  bool hasLink(const size_t) const;
  size_t nLinks() const;
  
  virtual bool isConnected(const gridUnit&) const =0;

  void setCyl(const int);
  void addCyl(const int);

  virtual std::string getShell() const;
  virtual std::string getInner() const;

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const gridUnit&);

}

#endif
 
