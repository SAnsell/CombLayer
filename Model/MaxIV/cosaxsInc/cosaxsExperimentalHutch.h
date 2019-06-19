/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Model/MaxIV/cosaxs/cosaxsExperimentalHutch.cxx
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov
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
#ifndef xraySystem_cosaxsExperimentalHutch_h
#define xraySystem_cosaxsExperimentalHutch_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class cosaxsExperimentalHutch
  \version 1.0
  \author K. Batkov
  \date June 2019
  \brief cosaxsExperimentalHutch unit  

  Experimental hutch with a penetration hole in the back wall
  for the CoSAXS submarine
*/

class cosaxsExperimentalHutch :
    public xraySystem::ExperimentalHutch
{
 private:
  double backHoleXStep;            ///< BackHole XStep [front wall]
  double backHoleZStep;            ///< BackHole ZStep  
  double backHoleRadius;           ///< BackHole radius

  virtual void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  cosaxsExperimentalHutch(const std::string&);
  cosaxsExperimentalHutch(const cosaxsExperimentalHutch&);
  cosaxsExperimentalHutch& operator=(const cosaxsExperimentalHutch&);
  virtual ~cosaxsExperimentalHutch();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
