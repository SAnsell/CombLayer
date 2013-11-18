/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/PlateTarget.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef ts1System_PlateTarget_h
#define ts1System_PlateTarget_h

class Simulation;

namespace ts1System
{

/*!
  \class PlateTarget
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief W/Ta plates 
*/

class PlateTarget : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int ptIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var
  
  double height;                 ///< Width tank
  double width;                 ///< Width tank
  
  size_t nBlock;                ///< nBlock
  std::vector<double> tBlock;   ///< Vector of block thickness
  std::vector<int> blockType;   ///< Vector of block types

  double taThick;               ///< Ta thickness
  double waterThick;            ///< water gap thickness
  double waterHeight;            ///< Width tank
  double WWidth;                ///< W Width [half]
  double WHeight;               ///< W Height [half]

  double backPlateThick;        ///< BackPlate thickness
  double IThick;                ///< BackPlate inset thickness
  double IWidth;                ///< BackPlate inset width
  double IOffset;               ///< BackPlate 
  double pinRadius;             ///< Backplate pin radius

  int feMat;                    ///< Fe material
  int taMat;                    ///< Ta material
  int wMat;                     ///< W material [def]
  int waterMat;                 ///< Water material

  double feedXOffset;           ///< Top ta plate cut point
  double feedHeight;
  double feedWidth;

  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces(const attachSystem::FixedComp&);
  void createLinks();
  void createObjects(Simulation&);

 public:

  PlateTarget(const std::string&);
  PlateTarget(const PlateTarget&);
  PlateTarget& operator=(const PlateTarget&);
  ~PlateTarget();

  Geometry::Vec3D plateEdge(const size_t,double&,double&) const;
  void buildFeedThrough(Simulation&);
  void populate(const Simulation&);  
  double getTargetLength() const;
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
