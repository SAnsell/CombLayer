/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/Bunker.h
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
#ifndef essSystem_Bunker_h
#define essSystem_Bunker_h

class Simulation;

namespace essSystem
{

  class BunkerMainWall;
  class BunkerRoof;
  class BunkerWall;
  class BunkerInsert;
  class Chicane;  
/*!
  \class Bunker
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bulk around Reflector
*/

class Bunker : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
   
  const int bnkIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  bool leftWallFlag;            ///< Build left wall
  bool rightWallFlag;           ///< Build right wall
  
  Geometry::Vec3D rotCentre;    ///< Rotation centre

  
  double leftPhase;              ///< Sector phase left
  double rightPhase;             ///< Sector phase right
  
  double leftAngle;              ///< Extent of left angle
  double rightAngle;             ///< Extent of right ange


  // MAIN WALL
  size_t nSectors;               ///< Number of sector divisions
  std::vector<double> sectPhase; ///< sector angles

  size_t nSegment;               ///< Number of sections in a segment
  std::vector<double> segDivide; ///< Segment divider

  size_t nVert;                  ///< Number of vertical divisions
  std::vector<double> vertFrac;  ///< Vertical fraction

  size_t nLayers;                ///< number of outgoing layers
  std::vector<double> wallFrac;  ///< thicknesss (fractions)

  /// Roof component
  std::shared_ptr<BunkerRoof> roofObj;              
  /// Roof component
  std::shared_ptr<BunkerWall> wallObj;              

  // SIDES:

  int sideFlag;                      ///< Which sides are divided [left/right]
  size_t nSide;                      ///< number of side layers
  std::vector<double> sideFrac;      ///< guide Layer thicknesss (fractions)
  size_t nSideVert;                  ///< number of side vert layers
  std::vector<double> sideVertFrac;  ///< side vert thicknesss (fractions)
  size_t nSideThick;                 ///< layers going low R to high R
  std::vector<double> sideThickFrac; ///< side low-high thicknesss (fractions)

  
  double innerRadius;            ///< inner radius [calculated]
  double wallRadius;             ///< Wall radius
  double floorDepth;             ///< Floor depth
  double wallHeight;             ///< Wall height (allows roof to intercolate)
  double roofHeight;             ///< Roof height

  double wallThick;              ///< backWall thickness
  double sideThick;              ///< Side thickness
  double roofThick;              ///< Roof thickness
  double floorThick;             ///< Floor thickness

  int voidMat;                   ///< void material 
  int wallMat;                   ///< wall material
  int roofMat;                   ///< roof material

  double midZ;                   ///< Mid z point

  // Bunker Material distribution:
  std::string loadFile;            ///< Bunker input file
  std::string outFile;             ///< Bunker output file

  void createWallSurfaces(const Geometry::Vec3D&,
			  const Geometry::Vec3D&);
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,const bool);

  void createSurfaces(const bool);
  void createLinks(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);

  void layerProcess(Simulation&);

  void createSideLinks(const Geometry::Vec3D&,const Geometry::Vec3D&,
		       const Geometry::Vec3D&,const Geometry::Vec3D&);


  void createMainWall(Simulation&);
  void createMainRoof(Simulation&,const int);

 public:

  Bunker(const std::string&);
  Bunker(const Bunker&);
  Bunker& operator=(const Bunker&);
  virtual ~Bunker();

  /// Roof component
  std::shared_ptr<BunkerRoof> getRoofObj() const { return roofObj; }

  void calcSegPosition(const size_t,Geometry::Vec3D&,
		       Geometry::Vec3D&,Geometry::Vec3D&,
		       Geometry::Vec3D&) const;
  std::string calcSegment(const Simulation&,const Geometry::Vec3D&,
			  const Geometry::Vec3D&) const;

  /// set centre rotation
  void setRotationCentre(const Geometry::Vec3D& RC)
    { rotCentre=RC; }
  void setCutWall(const bool,const bool);

  void cutInsert(Simulation&,const BunkerInsert&) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const bool);


  
};

}

#endif
 

