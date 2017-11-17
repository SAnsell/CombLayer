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

  /// Link storage type
  typedef std::set< std::pair<std::string,std::string> > BeamTYPE;
  const int rodIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  /// Information for each centre
  std::map<std::string,pillarInfo> PInfo;
  
  double width;                ///< X-beam length
  double depth;                ///< Along beam length
  double thick;                ///< metal thickness
  int mat;                     ///< Matieral
  int innerMat;                ///< Inner filling material

  double topFootHeight;        ///< Height of top feet [total]
  double topFootDepth;         ///< Depth (along beam) of top feet
  double topFootWidth;         ///< X-beam width of top feet
  double topFootThick;         ///< Thickness of filled metal 
  double topFootGap;           ///< Clearance gap

  double beamWidth;            ///< Total beam width
  double beamWallThick;        ///< Wall thick in beam
  double beamRoofThick;        ///< Top thick in beam
  double beamWallGap;          ///< Gap round wall

  double longWidth;            ///< Total beam width
  double longWallThick;        ///< Wall thick in beam
  double longRoofThick;        ///< Wall thick in beam
  double longWallGap;          ///< Gap round wall

  size_t nCrossBeam;           ///< Number of cross beam
  BeamTYPE beamLinks;          ///< link of each X-beamline
  size_t nLongBeam;            ///< Number of long beam
  BeamTYPE longLinks;          ///< link of each long-beamline

  ///< roof object
  std::shared_ptr<attachSystem::CellMap> roofCells; 
  
  HeadRule topFoot;            ///< Full headrule
  HeadRule topFootPlate;       ///< Plate headrule
  HeadRule topBeam;            ///< Top beam inner
  HeadRule baseBeam;           ///< Base beam inner
  HeadRule topLong;            ///< Top long  inner
  HeadRule baseLong;           ///< Base long inner

  
  void populate(const FuncDataBase&);
  void populateBeamSet(const FuncDataBase&,const std::string&,
		       const long int,const long int,BeamTYPE&) const;
			      
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  void createCrossBeams(Simulation&);
  void createLongBeams(Simulation&);
  
  void insertPillars(Simulation&,const attachSystem::CellMap&);
  
  void insertBeamCells(Simulation&,
		       const double,
		       const Geometry::Vec3D&,
		       const std::array<Geometry::Vec3D,2>&,
		       const std::string&);
  
  void insertPillarCells(Simulation&,const pillarInfo&,const std::string&);

  void insertRoofCells(Simulation&,
		       const std::array<Geometry::Vec3D,4>&,
		       const double,
		       const std::string&);

  
  void getPillarPair(const std::string&,const std::string&,
		     Geometry::Vec3D&,Geometry::Vec3D&,
		     Geometry::Vec3D&,int&,int&) const;
  void createBeamSurfaces(const int,const Geometry::Vec3D&,
			  const Geometry::Vec3D&,const double,
			  const double,const double);
  void createBeamObjects(Simulation&,
			 const int,const std::string&,
			 const HeadRule&,const HeadRule&,
			 const double,const Geometry::Vec3D&,
			 const std::array<Geometry::Vec3D,2>&);
  
  
  HeadRule getCrossPillarBoundary
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     const int,const int,std::array<Geometry::Vec3D,2>&) const;
  HeadRule getLongPillarBoundary
    (const Geometry::Vec3D&,const Geometry::Vec3D&,
     const int,const int,std::array<Geometry::Vec3D,2>&) const;

  HeadRule getInterPillarBoundary
    (const int,const int,const Geometry::Vec3D&,const Geometry::Vec3D&,
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
 

