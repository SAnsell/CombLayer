/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/BilbaoWheelInnerStructure.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_BilbaoWheelInnerStructure_h
#define essSystem_BilbaoWheelInnerStructure_h

class Simulation;

namespace essSystem
{
/*!
  \class BilbaoWheelInnerStructure
  \author K. Batkov
  \version 1.0
  \date Nov 2015
  \brief Inner structure of Bilbao Wheel (engineering details)
*/

class BilbaoWheelInnerStructure : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int insIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xyAngle;                ///< xy angle (mainly used to place the sector with bricks in the correct place). Can't rotate the wheel by itself since ProtonTube uses its coordinate system (\todo fix it)

  double temp;                    ///< Temperature (obtained from Inner cell of BilbaoWheel)
  double brickLen;                ///< Tungsten brick length (in radial direction)
  double brickWidth;              ///< Tungsten brick width
  int    brickMat;                ///< Tungsten brick material
  double brickGapLen;             ///< Distance between bricks in radial direction
  double brickGapWidth;           ///< Distance between bricks in normal to radial direction
  int    brickGapMat;             ///< Material of gap between bricks

  int    nSectors;                ///< Number of sectors in Tungsten
  double secSepThick;             ///< Thickness of sector separator
  int    secSepMat;               ///< Material of sector separator

  int    nBrickSectors;           ///< number of sectors filled with bricks
  int    nBrickLayers;            ///< number of radial brick layers
  std::vector<int>  nBricks;      ///< number of bricks in each radial layer
  int    nSteelLayers;            ///< number of brick layers made of steel (counting from internal cylinder)
  int    brickSteelMat;           ///< Steel brick material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&, attachSystem::FixedComp&);
  void createLinks();

  void createBrickSurfaces(const attachSystem::FixedComp&,
			   const Geometry::Plane*, const Geometry::Plane*, const int);
  void createBricks(Simulation&, attachSystem::FixedComp&,
		    const std::string, const std::string, const int);

  // polar angle of the given sector's centre. Clockwise starting from -Y
  inline double getSectorAngle(int i) const { return (2*(i+1)-1)*M_PI/nSectors * 180.0/M_PI; }

  double sideIntersect(const std::string&, const Geometry::Plane*);

 public:

  BilbaoWheelInnerStructure(const std::string&);
  BilbaoWheelInnerStructure(const BilbaoWheelInnerStructure&);
  BilbaoWheelInnerStructure& operator=(const BilbaoWheelInnerStructure&);
  virtual BilbaoWheelInnerStructure* clone() const;
  virtual ~BilbaoWheelInnerStructure();

  void createAll(Simulation&,attachSystem::FixedComp&);

};

}

#endif
 
