/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/RefBolts.h
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
#ifndef moderatorSystem_RefBolts_h
#define moderatorSystem_RefBolts_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class RefBolts
  \version 1.0
  \author S. Ansell
  \date October 2011
  \brief RefBolts [insert object]
*/

class RefBolts : public attachSystem::FixedComp
{
 private:
  
  const int boltIndex;          ///< Index of surface offset
  
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createBoltGrp(Simulation&,const std::string&);

 public:

  RefBolts(const std::string&);
  RefBolts(const RefBolts&);
  RefBolts& operator=(const RefBolts&);
  ~RefBolts();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
