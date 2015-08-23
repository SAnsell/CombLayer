/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/ExtControl.h
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
#ifndef physicsSystem_ExtControl_h
#define physicsSystem_ExtControl_h

namespace physicsSystem
{
 
/*!
  \class ExtControl
  \version 2.0
  \date April 2015
  \author S.Ansell
  \brief Process Exponential Transform card [EXT]  
*/

class ExtControl 
{
 private:

  typedef MapSupport::Range<int> RTYPE;             ///< Range type
  typedef std::map<RTYPE,EUnit> MTYPE;              ///< Master type

  std::set<std::string> particles;           ///< Particle list
  
  /// cells : Exp card values
  std::map<MapSupport::Range<int>,EUnit> MapItem; 
  std::map<size_t,Geometry::Vec3D> CentMap;  ///< Location vectors 

  /// maps NEW -> OLD
  std::map<int,int> renumberMap;
    
  void writeHeader(std::ostream&) const;
  
 public:
   
  ExtControl();
  ExtControl(const ExtControl&);
  ExtControl& operator=(const ExtControl&);
  virtual ~ExtControl();

  void clear();

  void addElm(const std::string&);
  int addUnitList(int&,const std::string&);

  int addUnit(const MapSupport::Range<int>&,
	      const std::string&);
  int addUnit(const int&,const std::string&);

  void setVect(const size_t,const Geometry::Vec3D&);
  size_t addVect(const Geometry::Vec3D&);
  void renumberCell(const int,const int);
  
  void write(std::ostream&,const std::vector<int>&,
	     const std::set<int>&) const;
  
};

}

#endif
