/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/RoofPillars.h
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
#ifndef essSystem_RoofPillars_h
#define essSystem_RoofPillars_h

class Simulation;

namespace essSystem
{
  class Bunker;

  
/*!
  \class RoofPillars
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Roof pillar to hold up bunker roof
*/

class RoofPillars : public attachSystem::FixedComp,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:
   
  const int rodIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  /// Name for each centre
  std::map<std::string,pillarInfo> PInfo;
  
  double width;                ///< X-beam length
  double depth;                ///< Along beam length
  double thick;                ///< metal thickness
  int mat;                     ///< Matieral

  double topFootHeight;        ///< Height of top feet [total]
  double topFootDepth;         ///< Depth (along beam) of top feet
  double topFootWidth;         ///< X-beam width of top feet
  double topFootThick;         ///< Thickness of filled metal 
  double topFootGap;           ///< Clearance gap

  double beamWidth;            ///< Total beam width
  double beamWallThick;        ///< Wall thick in beam
  
  size_t nCrossBeam;           ///< Number of cross beam
  /// link of each beamline
  std::vector< std::pair<std::string,std::string> > beamLinks;
  
  HeadRule topFoot;            ///< Full headrule
  HeadRule topFootPlate;       ///< Plate headrule
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  void createCrossBeams(Simulation&);
  
  void insertPillars(Simulation&,const attachSystem::CellMap&);
  void insertBeamCells(Simulation&,const Geometry::Vec3D&,
		       const Geometry::Vec3D&,
		       const std::array<Geometry::Vec3D,2>&,
		       const std::string&);
  void insertPillarCells(Simulation&,const pillarInfo&,const std::string&);
  void insertRoofCells(Simulation&,const std::array<Geometry::Vec3D,4>&,
		       const std::string&);
  

  HeadRule getInterPillarBoundary
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     const int,const int,std::array<Geometry::Vec3D,2>&) const;
    
 public:

  RoofPillars(const std::string&);
  RoofPillars(const RoofPillars&);
  RoofPillars& operator=(const RoofPillars&);
  virtual ~RoofPillars();
  
  //  void setSimpleSurf(const int,const int);
  //  void setTopSurf(const attachSystem::FixedComp&,const long int);
  //  void setBaseSurf(const attachSystem::FixedComp&,const long int);

  void createAll(Simulation&,const Bunker&);

};

}

#endif
 

