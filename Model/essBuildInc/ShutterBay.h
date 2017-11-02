/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/ShutterBay.h
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
#ifndef essSystem_ShutterBay_h
#define essSystem_ShutterBay_h

class Simulation;

namespace essSystem
{

/*!
  \class ShutterBay
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief GuideUnit volume
*/

class ShutterBay : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int bulkIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double radius;        ///< radius of outer
  double height;        ///< height
  double depth;         ///< Depth
  double skin;          ///< radius of void surround
  double topSkin;        ///< top gap thickness 
  double cutSkin;        ///< cut gap skin in top cut

  double topCut;        ///< Distance below top height for cut
  double topRadius;     ///< Outer Radius at cut section

  size_t NCurtain;                ///< Number of layers in curtain block
  std::vector<double> curLayer;   ///< Vertial layers in curtain block
  std::vector<int> curMat;        ///< Material for curtain block
  
  int mat;              ///< Material
  int skinMat;          ///< Skin Material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);

 public:

  ShutterBay(const std::string&);
  ShutterBay(const ShutterBay&);
  ShutterBay& operator=(const ShutterBay&);
  virtual ~ShutterBay();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 

