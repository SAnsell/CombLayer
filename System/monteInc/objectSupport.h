/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/objectSupport.h
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
#ifndef tallySystem_objectSupport_h
#define tallySystem_objectSupport_h

class Simulation;

namespace mainSystem
{
  class inputParam;
}

/*!
  \namespace objectSupport
  \version 1.0
  \author S. Ansell
  \date July 2017
  \brief Support finding cells of a given type
*/

namespace objectSupport
{

  template<typename T> void addItem(T&,const int);

  template<typename T>
  T cellSelection(const Simulation&,const std::string&,const T&);
  
  
  std::vector<int> getCellSelection(const Simulation&,
				    const int,
				    const std::string&);
}

#endif
 
