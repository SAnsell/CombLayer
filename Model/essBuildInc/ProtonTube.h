/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/ProtonTube.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Konstantin Batkov
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
#ifndef essSystem_ProtonTube_h
#define essSystem_ProtonTube_h

class Simulation;

namespace essSystem
{
  class PBW;
  class TelescopicPipe;

/*!
  \class ProtonTube
  \author Konstantin Batkov / Stuart Ansell
  \version 2.0
  \date Apr 2018
  \brief proton beam tube
*/

class ProtonTube :  public attachSystem::CopiedComp,
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffsetUnit,
  public attachSystem::FrontBackCut
{
 private:
  
  int engActive; ///< True if engineering details (like PBW) should be built
  std::shared_ptr<TelescopicPipe> tube; ///< proton beam tube
  std::shared_ptr<PBW> pbw; ///< Proton beam window

  void populate(const FuncDataBase&);

 public:

  ProtonTube(const std::string&);
  ProtonTube(const ProtonTube&);
  ProtonTube& operator=(const ProtonTube&);
  virtual ~ProtonTube();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);

};
 
}

#endif

