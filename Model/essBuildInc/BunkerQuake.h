/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/BunkerQuake.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_BunkerQuake_h
#define essSystem_BunkerQuake_h

class Simulation;

namespace essSystem
{
  class Bunker;
  class BunkerMainWall;
  class BunkerInsert;
  class BunkerQUnit;
  
/*!
  \class BunkerQuake
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bunker roof 
*/

class BunkerQuake : public attachSystem::ContainedComp,
  public attachSystem::FixedComp
{
 private:

  const int cutIndex;             ///< Surface index
  int cellIndex;                  ///< Cell index

  std::vector<std::shared_ptr<BunkerQUnit> > QUnit;
 public:

  BunkerQuake(const std::string&);
  BunkerQuake(const BunkerQuake&);
  BunkerQuake& operator=(const BunkerQuake&);
  virtual ~BunkerQuake();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const long int);

};

}

#endif
 

