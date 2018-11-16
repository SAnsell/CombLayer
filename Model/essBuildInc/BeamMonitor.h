/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/PBW.h
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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
#ifndef essSystem_BeamMonitor_h
#define essSystem_BeamMonitor_h

class Simulation;

namespace essSystem
{

/*!
  \class BeamMonitor
  \author S. Ansell
  \version 1.0
  \date February 2013
  \brief BeamMonitor
*/

class BeamMonitor : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:

  size_t nSec;                   ///< Number of main segments
  std::vector<double> radius;    ///< Radius of each secion
  std::vector<double> thick;     ///< Thickness
  std::vector<int> mat;          ///< Material     
  double halfThick;              ///< Mid point thickness

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedGroup&,
		     const std::string&);
  void createLinks();
  
  std::string calcExclude(const size_t,
			  const attachSystem::ContainedGroup&,
			  const std::string&) const;


 public:

  BeamMonitor(const std::string&);
  BeamMonitor(const BeamMonitor&);
  BeamMonitor& operator=(const BeamMonitor&);
  virtual ~BeamMonitor();
   

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const attachSystem::ContainedGroup&,
		 const std::string&);
 
};

}

#endif
 
