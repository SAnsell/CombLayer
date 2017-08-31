/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/BeamTubeJoiner.h
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
#ifndef delftSystem_BeamTubeJoiner_h
#define delftSystem_BeamTubeJoiner_h

class Simulation;

namespace delftSystem
{

/*!
  \class BeamTubeJoiner
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief BeamTubeJoiner for reactor
  
  This is a Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class BeamTubeJoiner : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int flightIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double waterStep;             ///< Forward water step
  double innerStep;             ///< Forward inner linder
  double length;                ///< Total length

  double innerRadius;           ///< Outer radius [minus wall]
  double innerWall;             ///< Wall thickness
  double outerRadius;           ///< Outer radius [minus wall]
  double outerWall;             ///< Wall thickness
  
  double interThick;            ///< shield thickness
  double interFrontWall;        ///< shield front thickness
  double interWallThick;        ///< shield thickness (container)
  double interYOffset;          ///< Distance from centre

  int interMat;                 ///< Inter Material number
  int wallMat;                  ///< Wall Material number
  int gapMat;                   ///< Material in the gap

  void populate(const FuncDataBase&);
  void populatePortals(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,const long int);
  void createLinks(const attachSystem::FixedComp&,const long int);

 public:

  BeamTubeJoiner(const std::string&);
  BeamTubeJoiner(const BeamTubeJoiner&);
  BeamTubeJoiner& operator=(const BeamTubeJoiner&);
  virtual ~BeamTubeJoiner();

  // Accessor to inner void cell
  //  int getInnerVoid() const { return innerVoid; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
