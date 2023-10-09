/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructInc/VacuumPipe.h
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
#ifndef constructSystem_VacuumPipe_h
#define constructSystem_VacuumPipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class VacuumPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief VacuumPipe unit
*/

class VacuumPipe :
  public GeneralPipe
{
 private:
  
  virtual void populate(const FuncDataBase&) override;
  void createObjects(Simulation&);
  void createLinks();

 public:

  VacuumPipe(const std::string&);
  VacuumPipe(const VacuumPipe&);
  VacuumPipe& operator=(const VacuumPipe&);
  ~VacuumPipe() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif
