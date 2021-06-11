/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/BeamWing.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef tdcSystem_BeamWing_h
#define tdcSystem_BeamWing_h

class Simulation;

namespace tdcSystem
{
/*!
  \class BeamWing
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief Plate inserted in object 
  
  Designed to be a quick plate to put an object into a model
  for fluxes/tallies etc
*/

class BeamWing :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:
    
  double width;             ///< Full Width
  double height;            ///< Full Height
  double depth;             ///< Full Depth 

  int mainMat;              ///< Material
  
  virtual void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  

 public:

  BeamWing(const std::string&);
  BeamWing(const std::string&,const std::string&);
  BeamWing(const BeamWing&);
  BeamWing& operator=(const BeamWing&);
  virtual ~BeamWing();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
