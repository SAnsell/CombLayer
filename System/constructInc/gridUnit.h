/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/gridUnit.h
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
  long int iA;                      ///< Index A
  long int iB;                      ///< Index B
  Geometry::Vec3D Centre;           ///< Centre 

  std::vector<gridUnit*> gridLink;  ///< Links
  std::vector<int> cylSurf;         ///< Cylinder numbers
  std::vector<int> surfKey;         ///< Surf keys
  int cellNumber;                   ///< Designated cell number

  /// surfaces that close boundary
  std::pair<size_t,size_t> boundaryClosed;
  std::string cutStr;               ///< External cutting surf
  
 public:

  gridUnit(const size_t,const long int,
	   const long int,const Geometry::Vec3D&);  
  gridUnit(const size_t,const long int,const long int,
	   const bool,const Geometry::Vec3D&);
  gridUnit(const gridUnit&);
  gridUnit& operator=(const gridUnit&); 
  virtual ~gridUnit() {}  ///< Destructor

  /// accessor to iA
  long int getAIndex() const { return iA; }
  /// accessor to iB
  long int getBIndex() const { return iB; }
  /// Index value 
  long int getIndex() const { return 1000*iA+iB; }
  /// Void cell
  bool isEmpty() const { return empty; }
  /// access cutter flag
  bool isCut() const { return cut; }
  /// access cutter string
  const std::string& getCut() const { return cutStr; }
  /// Access centre
  const Geometry::Vec3D& getCentre() const { return Centre; }

  /// accessor to links
  const gridUnit* getLink(const size_t index) const
  { return gridLink[index % gridLink.size()]; }
  gridUnit* getLink(const size_t index) 
  { return gridLink[index % gridLink.size()]; }
  /// accessor to surface numbers
  int getSurf(const size_t index) const
  { return surfKey[index % surfKey.size()]; }
  
  /// Simple setter
  void setCutString(const std::string& S) { cutStr=S; }
  void setLink(const size_t,gridUnit*);
  void setSurf(const size_t,const int); 
  void clearLinks();
  bool isComplete() const;
  bool hasLink(const size_t) const;
  bool hasSurfLink(const size_t) const;
  size_t nLinks() const;

  ///\cond ABSTRACT
  virtual int gridIndex(const size_t) const =0;
  virtual bool isConnected(const gridUnit&) const =0;
  ///\endcond ABSTRACT
  
  void setCyl(const int);
  void addCyl(const int);
  /// access flag  and boundary

  void clearBoundary();
  void setBoundary(const size_t,const size_t);
  /// Accessor
  const std::pair<size_t,size_t>&
    getBoundary() const { return boundaryClosed; }
  /// determine if se have work to do
  bool isBoundary() const
    { return (boundaryClosed.first && boundaryClosed.second); }
  int clearBoundary(const size_t);
  
  virtual std::string getShell() const;
  virtual std::string getInner() const;

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const gridUnit&);

}

#endif
 
