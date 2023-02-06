/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2BuildInc/GPreMod.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef moderatorSystem_GPreMod_h
#define moderatorSystem_GPreMod_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class GPreMod
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief GGPreMod [insert object]
*/

class GPreMod :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::SurfMap
{
 private:

  double width;             ///< Total Width
  double height;            ///< Total height
  double depth;             ///< Total depth

  double alThickness;       ///< Al wall thickness

  double modTemp;           ///< Moderator material
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  GPreMod(const std::string&);
  GPreMod(const GPreMod&);
  GPreMod& operator=(const GPreMod&);
  virtual ~GPreMod();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
