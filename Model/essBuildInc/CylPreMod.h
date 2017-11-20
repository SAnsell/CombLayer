/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/CylPreMod.h
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
#ifndef essSystem_CylPreMod_h
#define essSystem_CylPreMod_h

class Simulation;

namespace essSystem
{
  class BlockAddition;
/*!
  \class CylPreMod
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder moderator
*/

class CylPreMod : public attachSystem::ContainedGroup,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index


  std::shared_ptr<BlockAddition> ExtAObj;     ///< Left Extension
  std::shared_ptr<BlockAddition> ExtBObj;     ///< Right Extension
  int blockActiveA;                           ///< Flag for left ext
  int blockActiveB;                           ///< Flag for right ext
  long int aSide;                               ///< Link point
  long int bSide;                               ///< Link point

  double innerRadius;             ///< Radius from inner cell
  double innerHeight;             ///< height from inner cell
  double innerDepth;              ///< Depth from inner cell

  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<double> depth;          ///< full depths
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures

  std::vector<Geometry::Vec3D> viewY; ///< Direction from centre
  std::vector<double> viewAngle;      ///< Angle from centre
  std::vector<double> viewOpenAngle;  ///< Angle opening
  std::vector<double> viewHeight;     ///< Height from centre
  std::vector<double> viewWidth;      ///< Width at intercept

  // Now calculated
  std::vector<Geometry::Vec3D> FLpts;   ///< Flight line corner 
  std::vector<Geometry::Vec3D> FLunit;  ///< Flight line direction  [-x,x,-z,z]
  std::vector<int> layerCells;          ///< Layered cells
  // Functions:  
  void checkItems(const attachSystem::FixedComp&);
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp*);
  void createLinks();
  
  void updateLayers(Simulation&,const char,
		    const size_t,const size_t) const;

  Geometry::Vec3D calcViewIntercept(const size_t,const long int) const;

 public:

  CylPreMod(const std::string&);
  CylPreMod(const CylPreMod&);
  CylPreMod& operator=(const CylPreMod&);
  virtual ~CylPreMod();

  void createAll(Simulation&,const attachSystem::FixedComp&);

  const std::shared_ptr<BlockAddition>& getBox(const char) const;
  std::string getBoxCut(const char) const;
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const long int) const;
  virtual int getLayerSurf(const size_t,const long int) const;
  virtual std::string getLayerString(const size_t,const long int) const;


};

}

#endif
 
