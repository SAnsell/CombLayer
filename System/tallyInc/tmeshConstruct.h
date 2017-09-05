/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/tmeshConstruct.h
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
#ifndef tallySystem_tmeshConstruct_h
#define tallySystem_tmeshConstruct_h

namespace attachSystem
{
  class FixedComp;
}

namespace mainSystem
{
  class inputParam;
}

class Simulation;

namespace tallySystem
{

/*!
  \class tmeshConstruct
  \version 1.0
  \author S. Ansell
  \date July 2012
  \brief Constructs a tmesh tally from inputParam
*/

class tmeshConstruct : public meshConstruct
{
 private:

	         
 public:

  tmeshConstruct();
  tmeshConstruct(const tmeshConstruct&);
  tmeshConstruct& operator=(const tmeshConstruct&);
  virtual ~tmeshConstruct() {}  ///< Destructor

  virtual void rectangleMesh(Simulation&,const int,
			     const std::string&,
			     const Geometry::Vec3D&,
			     const Geometry::Vec3D&,
			     const std::array<size_t,3>&) const;

  virtual void writeHelp(std::ostream&) const;
};

}

#endif
 
