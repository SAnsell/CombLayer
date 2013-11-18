/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/AlterSurfTS1.h
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
#ifndef AlterSurfTS1_h
#define AlterSurfTS1_h

/*!
  \class AlterSurfTS1
  \brief Class to hold a map of the surfaces that need to be changed
  \version 1.0
  \date May 2006
  \author S. Ansell

  Holds the slist of surface->surface conversions
  and the list of surfaces to erase.
*/

class AlterSurfTS1 : public AlterSurfBase
{
 private:

  void setMap();
  void setMapZero();

 public:

  AlterSurfTS1(FuncDataBase&);
  AlterSurfTS1(const AlterSurfTS1&);
  AlterSurfTS1& operator=(const AlterSurfTS1&);
  /// Clone constructor
  virtual AlterSurfTS1* clone() const { return new AlterSurfTS1(*this); } 
  virtual ~AlterSurfTS1();
  /// Accessor
  virtual std::string className() const { return "AlterSurfTS1"; }
    
};

#endif 
