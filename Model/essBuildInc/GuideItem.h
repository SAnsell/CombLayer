/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/GuideItem.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef essSystem_GuideItem_h
#define essSystem_GuideItem_h

class Simulation;

namespace essSystem
{

/*!
  \class GuideItem
  \version 1.0
  \author S. Ansell
  \date November 2013
  \brief Simple Guide element Housing + void
*/

class GuideItem :
    public attachSystem::ContainedGroup,
    public attachSystem::FixedRotateGroup,
    public attachSystem::CellMap
{
private:
  
  const std::string baseName;   ///< Base keyname
  int active;                   ///< Build/don't build flag
  
  double beamXStep;             ///< Beam port X-offset
  double beamZStep;             ///< Beam port Z-offset
  double beamXYAngle;           ///< Beam port Z-offset
  double beamZAngle;            ///< Beam angle 
  double beamWidth;             ///< Beam width 
  double beamHeight;            ///< Beam depth

  double sideGap;               ///< Clearance gap
  double topGap;                ///< Top gap 
  double baseGap;               ///< Base Gap
  
  size_t nSegment;              ///< number of segments in the insert
  std::vector<double> height;        ///< away from target
  std::vector<double> depth;         ///< towards target depth
  std::vector<double> width;         ///< Depth 
  std::vector<double> length;        ///< Y Axis length [last one ignored]
  int mat;                           ///< Material
  int filled;                        ///< Inner void filled

  int dividePlane;     ///< Divide plane
  int innerCyl;        ///< Inner Cylinder
  int outerCyl;        ///< Outer Cylinder 
  double RInner;       ///< Inner cylinder radius 
  double ROuter;       ///< Outer cylinder radius 

  const GuideItem* GPtr;     ///< Neighbour unit
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);
  std::string sideExclude(const size_t) const;
  std::string getEdgeStr(const GuideItem*) const;
  const Geometry::Plane* getPlane(const int) const;
  
 public:

  GuideItem(const std::string&,const size_t);
  GuideItem(const GuideItem&);
  GuideItem& operator=(const GuideItem&);
  virtual ~GuideItem();

  
  /// accessor to active status
  int isActive() const { return active; }
  /// set active neighbour
  void setNeighbour(const GuideItem* GP) { GPtr=GP; }
  
  void setCylBoundary(const int,const int,const int);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 

