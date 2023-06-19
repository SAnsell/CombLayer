/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/CylContainer.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::LayerComp
{
 private:

  std::vector<double> radius;         ///< cylinder radii
  std::vector<double> height;         ///< Full heights
  std::vector<int> mat;               ///< Materials
  std::vector<double> temp;           ///< Temperatures


  int mainCell;                      ///< Main cell [centre]
  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CylContainer(const std::string&);
  CylContainer(const CylContainer&);
  CylContainer& operator=(const CylContainer&);
  ~CylContainer() override;
  virtual CylContainer* clone() const;
  
  /// Accessor to the main H2 body
  virtual int getMainBody() const { return buildIndex+1; }

  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const override;
  HeadRule getCommonSurf(const long int) const override;
  HeadRule getLayerHR(const size_t,const long int) const override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
