/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/BeamInsert.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef delftSystem_BeamInsert_h
#define delftSystem_BeamInsert_h

class Simulation;

namespace delftSystem
{

/*!
  \class BeamInsert
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief BeamInsert for reactor
  
  This is a twin object Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class BeamInsert : 
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp
{
 private:
  
  
  double length;                ///< Total length
  double radius;                ///< Outer radius [minus wall]

  size_t nSlots;          ///< Number of holes
  std::vector<std::shared_ptr<beamSlot> > Holes;  ///< Hole Radius

  int mat;                      ///< Inter Material number

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeamInsert(const std::string&);
  BeamInsert(const BeamInsert&);
  BeamInsert& operator=(const BeamInsert&);
  ~BeamInsert() override;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
