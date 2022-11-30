/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuildInc/ralVariableSetup.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef ralVariableSetup_h
#define ralVariableSetup_h


namespace mainSystem
{
  class inputParam;
}

namespace setVariable
{
  void RefBolts(FuncDataBase&);


  void TS1base(FuncDataBase&);
  void TS1BlockTarget(FuncDataBase&);
  void TS1BulletTarget(FuncDataBase&);
  void TS1CylTarget(FuncDataBase&);
  void TS1PlateTarget(FuncDataBase&);
  void TS1InnerTarget(FuncDataBase&);
  void TS1Cannelloni(FuncDataBase&);
  void TS1CylFluxTrap(FuncDataBase&);
  void TS1CylFluxTrapReal(FuncDataBase&);
  void TS1EllipticCylTarget(FuncDataBase&);
  void TS1CylMod(FuncDataBase&);

  void TS1layout(FuncDataBase&);
  void TS2layout(FuncDataBase&);
  void TS1real(FuncDataBase&);
  void TS1engineer(FuncDataBase&);
  void TS1upgrade(FuncDataBase&);

  void TS2model(FuncDataBase&);
  void TS3model(FuncDataBase&);
  void ZoomVariables(const mainSystem::inputParam&,FuncDataBase&);
}

#endif
