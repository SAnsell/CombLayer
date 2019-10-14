/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/Bellows.h
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef constructSystem_Bellows_h
#define constructSystem_Bellows_h

class Simulation;

namespace constructSystem
{

/*!
  \class Bellows
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief Bellows unit [simplified round pipe]
*/

class Bellows :
  public SplitFlangePipe
{

 public:

  Bellows(const std::string&);
  Bellows(const Bellows&);
  Bellows& operator=(const Bellows&);
  virtual ~Bellows();
  
};

}

#endif
 
