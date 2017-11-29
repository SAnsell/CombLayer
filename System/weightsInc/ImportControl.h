/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/ImportControl.h
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
#ifndef WeightSystem_ImportControl_h
#define WeightSystem_ImportControl_h

///\file 

namespace mainSystem
{
  class inputParam;
}

class Simulation;

namespace WeightSystem
{ 
  void zeroImp(Simulation&,const int,const int);
  void simulationImp(Simulation&,const mainSystem::inputParam&);
  void FCL(Simulation&,const mainSystem::inputParam&);
  void IMP(Simulation&,const mainSystem::inputParam&);
  void ExtField(Simulation&,const mainSystem::inputParam&);
  void EnergyCellCut(Simulation&,const mainSystem::inputParam&);
  void DXT(Simulation&,const mainSystem::inputParam&);
  void PWT(Simulation&,const mainSystem::inputParam&);
  void SBias(Simulation&,const mainSystem::inputParam&);
  void removePhysImp(Simulation&,const std::string&);
  void setWImp(Simulation&,const std::string&);
  void clearWImp(Simulation&,const std::string&);

}


#endif
 
