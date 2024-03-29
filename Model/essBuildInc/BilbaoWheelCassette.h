/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/BilbaoWheelCassette.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell / Konstantin Batkov
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
#ifndef essSystem_BilbaoWheelCassette_h
#define essSystem_BilbaoWheelCassette_h

class Simulation;

namespace essSystem
{

/*!
  \class BilbaoWheelCassette
  \version 1.0
  \author Konstantin Batkov
  \date 21 Aug 2017
  \brief Bilbao wheel sector cassette
*/

class BilbaoWheelCassette :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut
{
 private:

  const std::string baseName;   ///< Base Name
  const std::string commonName; ///< Template (part between wheel name and sector number)

  int bricksActive;            ///< True if bricks are active

  double rotAngle;              ///< Rotation angle
  double wallThick;             ///< Side wall thickness
  double delta;                 ///< Angular width [deg]
  double temp;                  ///< Temperature

  size_t   nWallSeg;            ///< Number of wall segments (for detailed geometry)
  /// Array of wall lengths.
  /// Sign defines direction of the corresponding groove (to/from the side wall).
  /// engActive=1: grooves are built;
  /// engActive=0: wall of constant average thickness is built,
  /// Thickness is calculated in order to have the same amount of steel
  /// as in the grooved wall.
  std::vector<double> wallSegLength;
  double wallSegDelta;          ///< Wall angular width [deg] (for detailed wall geometry)
  double wallSegThick;          ///< Wall base thickness (for detailed wall geometry)

  int homoWMat;     ///< Tungsten material for homogenized W bricks
  int homoSteelMat; ///< Steel material for homogenized steel bricks
  int wallMat;                  ///< wall material
  int heMat;                    ///< Helium material
  long floor;                   ///< Floor link point
  long roof;                    ///< Roof link point
  long back;                    ///< Back cylinder link point
  long front;                   ///< Front cylinder link point
  std::vector<size_t> nBricks;     ///< Number of bricks in each segment
  double brickWidth; ///< Brick width
  double brickLength; ///< Brick length
  double brickGap; ///< Distance between bricks in the given segment
  int brickSteelMat; ///< Steel Brick material
  int brickWMat; ///< Tungsten Brick material
  size_t nSteelRows; ///< Number of brick rows made from steel
  double pipeCellThick; ///< Thickness of the pipe cell
  int pipeCellMat; ///< Pipe cell homogenized material

  double innerCylRadius; // radius of inner cylinder

  double getSegWallArea() const;
  double getSegWallThick() const;
  double getBrickGapThick(const size_t&) const;
  void   buildBricks();

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createSurfacesBricks();
  void createObjects(Simulation&);
  void createObjectsBricks(Simulation&);
  void createLinks();
  

 public:

  BilbaoWheelCassette(const std::string&,const std::string&,const size_t&);
  BilbaoWheelCassette(const BilbaoWheelCassette&);
  BilbaoWheelCassette& operator=(const BilbaoWheelCassette&);
  virtual BilbaoWheelCassette* clone() const;
  ~BilbaoWheelCassette() override;

  void setRotAngle(const double XY) { rotAngle=XY; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif


