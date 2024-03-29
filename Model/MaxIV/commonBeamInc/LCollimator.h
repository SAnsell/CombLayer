 /********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/LCollimator.h
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
#ifndef xraySystem_LCollimator_h
#define xraySystem_LCollimator_h

class Simulation;

namespace xraySystem
{
  /*!
    \class LCollimator
    \version 1.0
    \author S. Ansell
    \date June 2015
    \brief Variable detemine hole type
  */
  
class LCollimator :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double width;                 ///< Main radius
  double height;                ///< Main height
  double length;                ///< thickness of collimator
  
  double innerAWidth;           ///< front width
  double innerAHeight;          ///< front height

  double innerBWidth;           ///< back width
  double innerBHeight;          ///< back height

  int mat;                      ///< material
  int voidMat;                  ///< void material

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  LCollimator(const std::string&);
  LCollimator(const LCollimator&);
  LCollimator& operator=(const LCollimator&);
  ~LCollimator() override {}  ///< Destructor

  void populate(const FuncDataBase&) override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
