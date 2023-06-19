/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photonInc/CylLayer.h
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
#ifndef photonSystem_CylLayer_h
#define photonSystem_CylLayer_h

class Simulation;

namespace photonSystem
{

/*!
  \struct LInfo
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Data unit for a layer in an object
*/

struct LInfo
{
  size_t nDisk;                ///< number of units
  double thick;                ///< Thickness
  std::vector<double> Radii;   ///< Radii
  std::vector<int> Mat;        ///< Material
  std::vector<double> Temp;    ///< Temperature
  
  void resize(const size_t);
};

/*!
  \class CylLayer
  \author S. Ansell
  \version 1.0
  \date Janurary 2015
  \brief Specialized for a layered cylinder pre-mod object
*/

class CylLayer :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut
{
 private:


  double outerRadius;                ///< Outer radius
  size_t nLayers;                    ///< Layer count
  std::vector<LInfo> LVec;           ///< Layer Info

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CylLayer(const std::string&);
  CylLayer(const CylLayer&);
  CylLayer& operator=(const CylLayer&);
  ~CylLayer() override;
  virtual CylLayer* clone() const;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
};

}

#endif
 
