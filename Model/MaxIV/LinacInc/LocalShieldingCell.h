/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/LocalShieldingCell.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef tdcSystem_LocalShieldingCell_h
#define tdcSystem_LocalShieldingCell_h

class Simulation;

namespace tdcSystem
{
  class LocalShielding;

/*!
  \class LocalShieldingCell
  \version 1.0
  \author Konstantin Batkov
  \date February 2021
  \brief LocalShieldingCell
*/

class LocalShieldingCell :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:

  typedef std::map<std::string,std::pair<std::string,std::string>> mapTYPE;
  
  /// Unit for the shielding
  std::vector<std::shared_ptr<tdcSystem::LocalShielding>> Units;
  std::map<std::string,std::pair<std::string,std::string>>
    connections;
  std::map<std::string,std::pair<std::string,std::string>>
    surfaces;
  
  void createObjects(Simulation&);
  void createLinks();

 public:

  LocalShieldingCell(const std::string&,const std::string&);
  LocalShieldingCell(const LocalShieldingCell&);
  LocalShieldingCell& operator=(const LocalShieldingCell&);
  virtual ~LocalShieldingCell();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,const long int);

};

}

#endif
