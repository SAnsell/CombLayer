/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   xrayHutchInc/DefUnitsXrayHut.h
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
#ifndef mainSystem_DefUnitsXrayHut_h
#define mainSystem_DefUnitsXrayHut_h

class Simulation;
class FuncDataBase;

namespace mainSystem
{
  class defaultConfig;
  class inputParam;

  void setDefUnits(FuncDataBase&,inputParam&);

}


#endif
 
