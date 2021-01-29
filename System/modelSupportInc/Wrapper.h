/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/Wrapper.h
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
#ifndef Wrapper_h
#define Wrapper_h

namespace ModelSupport
{
  /*!
    \class Wrapper
    \version 1.0
    \author S. Ansell
    \date January 2010
    \brief Allows a cell to be wrapped (inside/outside)
  */

class Wrapper
{
 private:
			
  int originalCell;            ///< cell to wrap 

  std::set<int> nonActive;     ///< Surfaces to avoid (+ve) 
  int surfOffset;              ///< Surface offset number
  int interiorFlag;            ///< Interior / exterior
  double Distance;             ///< Distance to expand by
  
  std::map<int,int> newSurf;   ///< new name / New surfaces 
  std::string CellOut;         ///< Out string 
  
 public:

  Wrapper();
  Wrapper(const int,const int);
  Wrapper(const Wrapper&);
  Wrapper& operator=(const Wrapper&);
  ~Wrapper();

  /// Set surface offset
  void setSurfOffset(const int I) { surfOffset=I; } 
  void addNonActive(const int);
  std::string createSurfaces(const std::string&,const double);  
  /// Access string
  const std::string& getCellOut() const { return CellOut; }

};

}

#endif
