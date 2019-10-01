/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/phitsTallyModification.h
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
#ifndef phitsSystem_phitsTallyModification_h
#define phitsSystem_phitsTallyModification_h

class Simulation;
class SimPHITS;

namespace phitsSystem
{

  std::set<phitsTally*>
    getActiveTally(SimPHITS&,const std::string&);


  int setParticle(SimPHITS&,const std::string&,
		     const std::string&);

  int setEnergy(SimPHITS&,const std::string&,
		const double,const double,
		const size_t,const bool);

  int setAngle(SimPHITS&,const std::string&,
	       const double,const double,
	       const size_t,const bool);
  
  int setParticleType(SimPHITS&,const int,const std::string&);

    
}  // namespace phitsSystem 

#endif
