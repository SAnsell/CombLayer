/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/WeightModification.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef WeightModification_h
#define WeightModification_h

class Simulation;


namespace WeightSystem
{
  void vecModReduction(Simulation&,const int,const int);
  void addForcedCollision(Simulation&,const double);
  void setNuclearZero(Simulation&,const int,const int);
}

#endif
