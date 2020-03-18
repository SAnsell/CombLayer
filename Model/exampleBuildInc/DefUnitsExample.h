/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exampleBuildInc/DefUnitsExample.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNExample FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef mainSystem_DefUnitsExample_h
#define mainSystem_DefUnitsExample_h

class Simulation;
class FuncDataBase;

namespace mainSystem
{
  class defaultConfig;
  class inputParam;

  void setDefUnits(FuncDataBase&,inputParam&);

  void setExampleEX1(defaultConfig&,const std::vector<std::string>&);
  void setLinacVacTube(defaultConfig&,const std::vector<std::string>&);
}


#endif
 
