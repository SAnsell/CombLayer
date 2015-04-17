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
  \brief Process the dbnc options
  
  Taken from document la-ur-13-23395
  
*/

class ExpControl 
{
 private:

  std::map<int,EUnit> MapItem;
  std::vector<Geometry::Vec3D> VecUnits
    
 public:
   
  ExpControl();
  ExpControl(const ExpControl&);
  ExpControl& operator=(const ExpControl&);
  virtual ~ExpControl();

  size_t keyType(const std::string&) const;
    
  void setActive(const std::string&,const int);

  template<typename T>
  void setComp(const std::string&,const T&);

  template<typename T>
  const T& getComp(const std::string&) const;

  void clear();
  void write(std::ostream&) const;   
};

}

#endif
