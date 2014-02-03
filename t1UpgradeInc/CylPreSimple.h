/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1UpgradeInc/CylPreSimple.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef ts1System_CylPreSimple_h
#define ts1System_CylPreSimple_h

class Simulation;

namespace ts1System
{
/*!
  \class CylPreSimple
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder moderator
*/

class CylPreSimple : public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 private:
  
  const int modIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double innerRadius;             ///< Radius from inner cell
  double innerHeight;             ///< height from inner cell
  double innerDepth;              ///< Depth from inner cell

  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<double> depth;          ///< full depths
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures

  // Now calculated
  size_t nView; 
  std::vector<Geometry::Vec3D> viewY;   ///< Flight line Y direction
  std::vector<Geometry::Vec3D> FLpts;   ///< Flight line corner 
  std::vector<Geometry::Vec3D> FLunit;  ///< Flight line direction  [-x,x,-z,z]
  // Functions:  
  void checkItems(const attachSystem::FixedComp&);
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedComp*);
  void createLinks();
  void createFlightPoint(const attachSystem::FixedComp&);

 public:

  CylPreSimple(const std::string&);
  CylPreSimple(const CylPreSimple&);
  CylPreSimple& operator=(const CylPreSimple&);
  virtual ~CylPreSimple();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&);

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;


};

}

#endif
 
