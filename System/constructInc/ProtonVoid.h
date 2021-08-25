/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/ProtonVoid.h
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
#ifndef ts1System_ProtonVoid_h
#define ts1System_ProtonVoid_h

class Simulation;

namespace ts1System
{

/*!
  \class ProtonVoid
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief ProtonVoid [insert object]
*/

class ProtonVoid :
    public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double viewRadius;            ///< Radius of proton tube
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const std::string&,const std::string&);

 public:

  ProtonVoid(const std::string&);
  ProtonVoid(const ProtonVoid&);
  ProtonVoid& operator=(const ProtonVoid&);
  ~ProtonVoid();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
