/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/beamTallyConstruct.h
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
#ifndef tallySystem_beamTallyConstruct_h
#define tallySystem_beamTallyConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace tallySystem
{

/*!
  \class beamTallyConstruct
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief Holds everthing for tallies

  Provides linkage to its outside on FixedComp[0]
*/

class beamTallyConstruct : virtual public pointConstruct
{
 private:
  
  static void calcBeamDirection(const attachSystem::FixedComp&,
				Geometry::Vec3D&,
				Geometry::Vec3D&);
			  
 public:

  beamTallyConstruct();
  beamTallyConstruct(const beamTallyConstruct&);
  beamTallyConstruct& operator=(const beamTallyConstruct&);
  virtual ~beamTallyConstruct() {}  ///< Destructor


  virtual void processPoint(Simulation&,const mainSystem::inputParam&,
		    const size_t) const;

  virtual void writeHelp(std::ostream&) const;

  // Additions:

  void addBeamLineTally(Simulation&,const int,const double,
			const std::string&,const long int,
			const double,const double) const;
  void addViewLineTally(Simulation&,const int,const double,
			const double,const double,const double) const;
  void addViewInnerTally(Simulation&,const int,const long int,const double,
			const double,const double,const double) const;
  void addShutterTally(Simulation&,const int,const double,
			const std::string&,const long int,
			const double,const double) const;

};

}

#endif
 
