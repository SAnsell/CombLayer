/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/MultiChannel.h
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
#ifndef constructSystem_MultiChannel_h
#define constructSystem_MultiChannel_h

class Simulation;

namespace constructSystem
{
  /*!
    \class MultiChannel
    \version 1.0
    \author S. Ansell
    \date January 2017
    \brief Multichannel reflector in a guide component
  */
  
class MultiChannel :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:
  
  const Geometry::Surface* baseSurf;
  const Geometry::Surface* topSurf;
  
  size_t nBlades;               ///< Number of blades
  double bladeThick;            ///< thickness of blade
  double length;                ///< thickness of collimator
  int bladeMat;                 ///< material of blades
  int voidMat;                  ///< material of void

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void processSurface(const size_t,const double,const double);
  void createObjects(Simulation&);
  void createLinks();

  
 public:
  
  MultiChannel(const std::string&);
  MultiChannel(const MultiChannel&);
  MultiChannel& operator=(const MultiChannel&);
  ~MultiChannel() override {}  ///< Destructor
  
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
