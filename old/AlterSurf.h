/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/AlterSurf.h
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
#ifndef AlterSurf_h
#define AlterSurf_h

/*!
  \class AlterSurf
  \brief Class to hold a map of the surfaces that need to be changed
  \version 1.0
  \date May 2006
  \author S. Ansell

  Holds the slist of surface->surface conversions
  and the list of surfaces to erase.
*/

class AlterSurf
{
 private:
  
  FuncDataBase& ConVar;            ///< Variable map
  std::map<int,int> AMap;          ///< The map of surface changes
  

  void setChange(const int,const int);
  void setMap();
  void setMapZero();

 public:

  AlterSurf(FuncDataBase&);
  AlterSurf(const AlterSurf&);
  AlterSurf& operator=(const AlterSurf&);
  ~AlterSurf();
  
  /// Access Map
  const std::map<int,int>& getMap() const 
    { return AMap; }
    
};

#endif 
