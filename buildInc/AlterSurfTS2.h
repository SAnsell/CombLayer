/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/AlterSurfTS2.h
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
#ifndef AlterSurfTS2_h
#define AlterSurfTS2_h

/*!
  \class AlterSurfTS2
  \brief Class to hold a map of the surfaces that need to be changed
  \version 1.0
  \date May 2006
  \author S. Ansell

  Holds the slist of surface->surface conversions
  and the list of surfaces to erase.
*/

class AlterSurfTS2 : public AlterSurfBase
{
 private:
    
  virtual void setMap();
  virtual void setMapZero();

 public:

  AlterSurfTS2(FuncDataBase&);
  AlterSurfTS2(const AlterSurfTS2&);
  AlterSurfTS2& operator=(const AlterSurfTS2&);
  /// Clone constructor
  virtual AlterSurfTS2* clone() const { return new AlterSurfTS2(*this); } 
  virtual ~AlterSurfTS2();
  /// Accessor
  virtual std::string className() const { return "AlterSurfTS2"; }
    
};

#endif 
