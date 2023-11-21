/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PipeCollimator.h
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
#ifndef constructSystem_PipeCollimator_h
#define constructSystem_PipeCollimator_h

class Simulation;

namespace constructSystem
{
  /*!
    \class PipeCollimator
    \version 1.0
    \author S. Ansell
    \date June 2015
    \brief Variable detemine hole type
  */
  
class PipeCollimator : 
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double length;                ///< thickness of collimator
  int mat;                      ///< material
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  PipeCollimator(const std::string&);
  PipeCollimator(const PipeCollimator&);
  PipeCollimator& operator=(const PipeCollimator&);
  ~PipeCollimator() override {}  ///< Destructor

  void populate(const FuncDataBase&) override;
  
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
  
};

}

#endif
 
