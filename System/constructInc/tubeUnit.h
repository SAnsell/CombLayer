/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   constructInc/tubeUnit.h
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
#ifndef shutterSystem_tubeUnit_h
#define shutterSystem_tubeUnit_h

namespace constructSystem
{

/*!
  \class tubeUnit
  \version 1.0
  \author S. Ansell
  \date October 2012
  \brief Deals with a number of tubes in a plate
 */

class tubeUnit
{
 private:
  
  /// Type of map to tubes
  typedef std::map<int,tubeUnit*> MTYPE; 
  /// type of map to planes
  typedef std::map<int,int> PTYPE;

  int empty;                        ///< Empty flag [1:empty:2:filled]
  int iA;                           ///< Index A
  int iB;                           ///< Index B
  Geometry::Vec3D Centre;           ///< Centre 

  std::vector<int> cylSurf;          ///< Centre surface
  std::map<int,int> linkPlanes;      ///< Link plane numbers [signed to enclose]
  std::map<int,tubeUnit*> linkPts;   ///< link points [random]
  
  static int getMapIndex(const size_t);

 public:

  tubeUnit(const int,const int,const Geometry::Vec3D&);
  tubeUnit(const tubeUnit&);
  tubeUnit& operator=(const tubeUnit&); 
  ~tubeUnit() {}  ///< Destructor

  void clearLinks();
  void addLink(tubeUnit*);
  bool isConnected(const tubeUnit&) const;
  bool isComplete(const size_t) const;
  void setEmpty() { empty=1; }        ///< Set empty
  void setFilled() { empty|=2; }      ///< Set Filled
  void setOutside() { empty|=4; }      ///< Set Filled
  bool isEmpty() const { return (empty & 1); }      ///< empty state
  bool isFilled() const { return (empty & 2); }      ///< filled state
  bool isOutside() const { return (empty & 4); }      ///< outer state
  /// Links
  size_t nLinks() const;
  std::string writeID() const;

  tubeUnit* getItem(const int,const int);
  const tubeUnit* getLink(const size_t) const;
  /// Centre point
  const Geometry::Vec3D& getCentre() const { return Centre; }
  bool midPlane(const size_t,Geometry::Vec3D&,Geometry::Vec3D&) const;
  

  void setCyl(const int);
  void addCyl(const int);
  void addPlane(const size_t,const int);

  std::string getCell(const ModelSupport::surfRegister&,
		      const size_t) const;

  void write(std::ostream&) const;

};

std::ostream&
operator<<(std::ostream&,const tubeUnit&);

}

#endif
 
