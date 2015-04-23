/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/tallyFactory.h
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
#ifndef tallySystem_tallyFactory_h
#define tallySystem_tallyFactory_h

/*!
  \namespace tallySystem
  \brief Holds this huge mess of tallies
  \author S. Ansell
  \version 1.0
  \date June 2006
  
*/

namespace tallySystem
{

/*!
  \class tallyFactory
  \brief creates instances of tallies
  \version 1.0
  \date May 2006
  \author S. Ansell
  
  This is a singleton class.
  It creates Tally* depending registered tallies
  and the key given. Predominately for creating
  tallies from an input deck where we only have the number.
*/

class tallyFactory
{
 private:

  typedef std::map<int,Tally*> MapType;     ///< Storage of tally pointers
  
  std::map<int,Tally*> TGrid;            ///< The tally stack

  tallyFactory();                       ///< singleton constructor
  tallyFactory(const tallyFactory&);

  /// Dummy assignment operator
  tallyFactory& operator=(const tallyFactory&);

  void registerTally();

 public:

  static tallyFactory* Instance();
  ~tallyFactory();
  
  Tally* createTally(const int) const;

};

}

#endif

