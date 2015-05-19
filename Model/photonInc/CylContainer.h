/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/CylContainer.h
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
#ifndef photonSystem_CylContainer_h
#define photonSystem_CylContainer_h

class Simulation;

namespace photonSystem
{

/*!
  \class CylContainer
  \author S. Ansell
  \version 1.0
  \date January 2015
  \brief Specialized for a cylinder container
*/

class CylContainer :
    public attachSystem::ContainedComp,
    public attachSystem::LayerComp,
    public attachSystem::FixedComp
{
 private:

  const int cylIndex;
  int cellIndex;

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< xy rotation angle
  double zAngle;                  ///< z rotation angle

  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures


  int mainCell;                      ///< Main cell [centre]
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CylContainer(const std::string&);
  CylContainer(const CylContainer&);
  CylContainer& operator=(const CylContainer&);
  virtual ~CylContainer();
  virtual CylContainer* clone() const;
  
  /// Accessor to the main H2 body
  virtual int getMainBody() const { return cylIndex+1; }

  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual int getCommonSurf(const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;

  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
