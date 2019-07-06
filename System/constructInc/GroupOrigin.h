/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:  constructInc/GroupOrigin.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef constructSystem_GroupOrigin_h
#define constructSystem_GroupOrigin_h

namespace constructSystem
{

/*!
  \class GroupOrigin
  \version 1.0
  \author S. Ansell
  \date February 2013
  \brief Group origin for two or more objects
*/

class GroupOrigin : public attachSystem::FixedOffset
{
 private:
  
 public:

  GroupOrigin(const std::string&);
  GroupOrigin(const GroupOrigin&);
  GroupOrigin& operator=(const GroupOrigin&);
  virtual ~GroupOrigin();
//

  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);
  
};

}

#endif
 
