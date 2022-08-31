/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/portBuilder.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef constructSystem_portBuilder_h
#define constructSystem_portBuilder_h

class Simulation;

namespace constructSystem
{
  class portItem;
  /*!
    \class portBuilder
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief factory class/method for building different port
  */
  
portItem*
makePortItem(const FuncDataBase&,
	       const std::string&,const std::string&);

portItem*
makePortItem(const FuncDataBase&,const std::string&);

 

void
populatePort(const FuncDataBase&,const std::string&,
	     std::vector<Geometry::Vec3D>&,
	     std::vector<Geometry::Vec3D>&,
	     std::vector<std::shared_ptr<portItem>>&);
 
}

#endif
 
