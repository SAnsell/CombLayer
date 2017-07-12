/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/BeamTube.h
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
#ifndef delftSystem_BeamTube_h
#define delftSystem_BeamTube_h

class Simulation;

namespace delftSystem
{

/*!
  \class BeamTube
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief BeamTube for reactor
  
  This is a Fixed for the primary build
  and Beamline to take acount of the track (inner build)
*/

class BeamTube : public attachSystem::ContainedComp,
  public attachSystem::FixedOffsetGroup,
  public attachSystem::CellMap
{
 private:
  
  const int flightIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double waterStep;             ///< Forward water step
  double innerStep;             ///< Forward inner linder
  double length;                ///< Total length
  double capRadius;             ///< Radius of cap [zero for flat]
  double innerRadius;           ///< Outer radius [minus wall]
  double innerWall;             ///< Wall thickness
  double outerRadius;           ///< Outer radius [minus wall]
  double outerWall;             ///< Wall thickness
  
  double frontWall;             ///< Front wall thickness
  double frontGap;              ///< Front wall gap
  double frontIWall;            ///< Inner wall thickness

  double interThick;            ///< shield thickness
  double interFrontWall;        ///< shield front thickness
  double interWallThick;        ///< shield thickness (container)
  double interYOffset;          ///< Distance from centre

  double portalRadius;          ///< Portal radius
  double portalThick;           ///< Portal thickness

  int innerMat;                 ///< Inner Material number in main void
  int interMat;                 ///< Inter Material number
  int wallMat;                  ///< Wall Material number
  int gapMat;                   ///< Material in the gap

  std::vector<double> portalOffset;     ///< Portal Offset 
  std::vector<int> portalMat;           ///< Portal material [if present]

  int innerVoid;                ///< Inner void cell

  void populate(const FuncDataBase&);
  void populatePortals(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createCapEndObjects(Simulation&);
  void createLinks();

 public:

  BeamTube(const std::string&);
  BeamTube(const BeamTube&);
  BeamTube& operator=(const BeamTube&);
  virtual ~BeamTube();

  // Accessor to inner void cell
  int getInnerVoid() const { return innerVoid; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
