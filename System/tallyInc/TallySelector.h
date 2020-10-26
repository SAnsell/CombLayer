/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/TallySelector.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef TallySelector_h
#define TallySelector_h

class SimMCNP;

bool checkLinkCells(const Simulation&,const int,const int);
bool constructCellMapPair(const Simulation&,
			  const std::string&,
			  const std::string&,
			  int&,int&);
bool
constructLinkRegion(const Simulation&,
		    const std::string&,
		    const std::string&,
		    int&,int&);

bool
constructSurfRegion(const Simulation&,
		    const std::string&,const std::string&,
		    const size_t,const size_t,
		    int&,int&);

void pointTallyWeights(SimMCNP&,const mainSystem::inputParam&);
void tallyModification(SimMCNP&,const mainSystem::inputParam&);
void tallyAddition(SimMCNP&,const mainSystem::inputParam&);

int beamTallySelection(SimMCNP&,const mainSystem::inputParam&);

int reactorTallySelection(SimMCNP&,const mainSystem::inputParam&);

#endif
 
