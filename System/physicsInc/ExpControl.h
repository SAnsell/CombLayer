/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physicsInc/ExpControl.h
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
#ifndef physicsSystem_ExpControl_h
#define physicsSystem_ExpControl_h

namespace physicsSystem
{
 
/*!
  \class ExpControl
  \version 1.0
  \date April 2015
  \author S.Ansell
  \brief Process Exponential Transform card [EXT]
*/

class ExpControl 
{
 private:

  std::set<std::string> particles;           ///< Particle list 
  std::map<int,EUnit> MapItem;               ///< cells : Exp card values
  std::map<size_t,Geometry::Vec3D> CentMap;  ///< Location vectors 

  void writeHeader(std::ostream&) const;
  
 public:
   
  ExpControl();
  ExpControl(const ExpControl&);
  ExpControl& operator=(const ExpControl&);
  virtual ~ExpControl();

  void clear();

  void addElm(const std::string&);
  int addUnit(const int,const std::string&);
  void addVect(const size_t,const Geometry::Vec3D&);
  void renumberCell(const int,const int);
  
  void write(std::ostream&,const std::vector<int>&) const;   
};

}

#endif
