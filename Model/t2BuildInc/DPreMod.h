/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2BuildInc/DPreMod.h
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
#ifndef moderatorSystem_DPreMod_h
#define moderatorSystem_DPreMod_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class DPreMod
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief GDPreMod [insert object]
*/

class DPreMod :
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

  double modTemp;           ///< Moderator temp
  int modMat;               ///< Moderator material
  int alMat;                ///< Al material
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  DPreMod(const std::string&);
  DPreMod(const DPreMod&);
  DPreMod& operator=(const DPreMod&);
  ~DPreMod() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
