/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/tallyConstructFactory.h
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
#ifndef tallySystem_tallyConstructFactory_h
#define tallySystem_tallyConstructFactory_h


namespace tallySystem
{

  class pointConstruct;
  class gridConstruct;
  class fmeshConstruct;
  class tmeshConstruct;
  class fluxConstruct;
  class heatConstruct;
  class itemConstruct;
  class surfaceConstruct;
  class fissionConstruct;
  class sswConstruct;
/*!
  \class tallyConstructFactory
  \brief Creates a 
  \version 1.0
  \date October 2013
  \author S. Ansell

  Class to determine to what complexity the tally constructors can
  have.
*/

class tallyConstructFactory
{
 private:

 public:

  tallyConstructFactory() {}       ///<  constructor
  ~tallyConstructFactory() {}      ///<  destructor
  
  pointConstruct* makePoint() const;
  gridConstruct* makeGrid() const;
  tmeshConstruct* makeTMesh() const;
  fmeshConstruct* makeFMesh() const;
  fluxConstruct* makeFlux() const;
  heatConstruct* makeHeat() const;
  itemConstruct* makeItem() const;
  surfaceConstruct* makeSurf() const;
  fissionConstruct* makeFission() const;
  sswConstruct* makeSSW() const;

};

}

#endif

