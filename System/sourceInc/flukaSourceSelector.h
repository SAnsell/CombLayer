/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/flukaSourceSelector.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef SDef_flukaSourceSelector_h
#define SDef_flukaSourceSelector_h

class Simulation;

namespace SDef
{
  void flukaSourceSelection(Simulation&,const mainSystem::inputParam&);
  void processPolarization(const mainSystem::MITYPE&,const std::string&);
}

#endif
 
