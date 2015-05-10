/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   weightsInc/ExpManager.h
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
#ifndef WeightSystem_ExpManager_h
#define WeightSystem_ExpManager_h

namespace physicsSystem
{

/*!
  \class ExpManager
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Controls the addition of weights to the system
*/

class ExpManager 
{
 private:  

  std::map<std::string,ExpControl> EVec;

 public:

  ExpManager();
  ExpManager(const ExpManager&);
  ExpManager& operator=(const ExpManager&);
  ~ExpManager();
  
  ExpControl& particleFind(const std::string&);
  void renumberCell(const int,const int);  
  void maskCell(const int);

  void write(std::ostream&) const;

};

}

#endif
 
