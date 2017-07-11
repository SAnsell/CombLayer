/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/reactorTallyConstruct.h
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
#ifndef tallySystem_reactorTallyConstruct_h
#define tallySystem_reactorTallyConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace tallySystem
{

/*!
  \class reactorTallyConstruct
  \version 1.0
  \author S. Ansell
  \date April 2014
  \brief Constructs a reactor Power tally
*/

class reactorTallyConstruct : virtual public fissionConstruct
{
 public:

  reactorTallyConstruct();
  reactorTallyConstruct(const reactorTallyConstruct&);
  reactorTallyConstruct& operator=(const reactorTallyConstruct&);
  virtual ~reactorTallyConstruct() {}  ///< Destructor

  virtual int processPower(Simulation&,const mainSystem::inputParam&,
			    const size_t) const;

  virtual void writeHelp(std::ostream&) const;
};

}

#endif
 
