/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geomInc/surfaceFactory.h
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
#ifndef surfaceFactory_h
#define surfaceFactory_h

namespace Geometry 
{
/*!
  \class surfaceFactory
  \brief creates instances of tallies
  \version 1.0
  \date May 2006
  \author S. Ansell
  
  This is a singleton class.
  It creates Surface* depending registered surfaces
  and the key given. Predominately for creating
  surface from an input deck where we only have the type.
*/


class surfaceFactory
{

 private:

  typedef std::map<std::string,int> MapTYPE;   ///< Storage of surface indexx
  
  MapTYPE wordGrid;                       ///< Work Identifiers
  MapTYPE keyGrid;                        ///< Short letter identifiers 

  surfaceFactory();                       ///< singleton constructor
  surfaceFactory(const surfaceFactory&);  ///< singleton copy constructor

  /// Dummy assignment operator
  surfaceFactory& operator=(const surfaceFactory&)   
    { return *this; } 

  Surface* surfaceIndex(const int) const;
  void registerSurface();

 public:

  static surfaceFactory& Instance();
  ~surfaceFactory();
  
  Surface* createSurface(const std::string&) const;
  Surface* createSurfaceID(const std::string&) const;
  Surface* processLine(const std::string&) const;
  int getIndex(const std::string&) const;

};


}   // NAMESPACE geometry

#endif
