/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/FlangeDome.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef constructSystem_FlangeDome_h
#define constructSystem_FlangeDome_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class FlangeDome
  \version 1.0
  \author S. Ansell
  \date March 2021
  \brief FlangeDome for capping ports/vac-tubes etc
*/

class FlangeDome :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double curveRadius;           ///< inner Radius of dome (extent)
  double curveStep;             ///< Step from flat start plane
  double plateThick;            ///< Thickness of plate
  double flangeRadius;          ///< Flange radius 

  int voidMat;                 ///< inner (Void) material
  int mat;                     ///< Main material  

  constructSystem::portSet PSet;        ///< Port set
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void createPorts(Simulation&);
  
 public:

  FlangeDome(const std::string&);
  FlangeDome(const FlangeDome&);
  FlangeDome& operator=(const FlangeDome&);
  virtual ~FlangeDome();

  const portItem& getPort(const size_t) const;

  virtual void insertInCell(MonteCarlo::Object&) const;
  virtual void insertInCell(Simulation&,const int) const;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
