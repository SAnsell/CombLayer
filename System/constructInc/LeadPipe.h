/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/LeadPipe.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef constructSystem_LeadPipe_h
#define constructSystem_LeadPipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class LeadPipe
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief LeadPipe unit [simplified round pipe]
*/

class LeadPipe :
  public SplitFlangePipe
{
  
 public:

  LeadPipe(const std::string&);
  LeadPipe(const LeadPipe&);
  LeadPipe& operator=(const LeadPipe&);
  virtual ~LeadPipe();
  
};

}

#endif
 
