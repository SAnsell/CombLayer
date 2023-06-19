/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   snsBuildInc/RefPlug.h
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
 * MERCHANTABILITY or FITNSNS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef snsSystem_RefPlug_h
#define snsSystem_RefPlug_h

class Simulation;

namespace snsSystem
{

/*!
  \class RefPlug
  \author S. Ansell
  \version 1.0
  \date October 2012
  \brief Specialized for a cylinder reflector
*/

class RefPlug :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::LayerComp
{
 private:
  
  double height;                  ///< Be height
  double depth;                  ///< Be depth

  std::vector<double> radius;         ///< cylinder radii  
  std::vector<double> temp;           ///< Temperatures
  std::vector<int> mat;               ///< Materials

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  RefPlug(const std::string&);
  RefPlug(const RefPlug&);
  RefPlug& operator=(const RefPlug&);
  ~RefPlug() override;
  virtual RefPlug* clone() const;
  

  Geometry::Vec3D getSurfacePoint(const size_t,const long int) const override;
  HeadRule getLayerHR(const size_t,const long int) const override;
  HeadRule getCommonSurf(const long int) const override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
