/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/LocalShielding.h
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
#ifndef tdcSystem_LocalShielding3_h
#define tdcSystem_LocalShielding3_h

class Simulation;

namespace tdcSystem
{

/*!
  \class LocalShielding3
  \version 1.0
  \author Konstantin Batkov
  \date August 2021
  \brief LocalShielding with inidividual penetrations for 3 pipes
*/

class LocalShielding;

class LocalShielding3 :
    public tdcSystem::LocalShielding
{
  std::vector<double> midHoleShieldHeight;  ///< mid hole shielding/void heights

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  LocalShielding3(const std::string&);
  LocalShielding3(const LocalShielding3&);
  LocalShielding3& operator=(const LocalShielding3&);
  virtual LocalShielding3* clone() const;
  virtual ~LocalShielding3();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
