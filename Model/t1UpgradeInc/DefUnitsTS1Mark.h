/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/DefUnitsTS1Mark.h
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
#ifndef mainSystem_DefUnitsTS1Mark_h
#define mainSystem_DefUnitsTS1Mark_h

class Simulation;
class FuncDataBase;

namespace mainSystem
{
  class inputParam;

  void setDefUnits(FuncDataBase&,inputParam&);

  void setTS1MarkIV(defaultConfig&); 
  void setTS1MarkV(defaultConfig&); 
  void setTS1MarkVI(defaultConfig&); 
  void setTS1MarkVIb(defaultConfig&);

}


#endif
 
