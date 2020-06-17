/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/MultiPipe.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_MultiPipe_h
#define tdcSystem_MultiPipe_h

class Simulation;

namespace tdcSystem
{
  class subPipeUnit;
  
/*!
  \class MultiPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief MultiPipe unit [simplified round pipe]
*/

class MultiPipe :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:
  
  double flangeRadius;
  double flangeLength;

  std::vector<subPipeUnit> pipes;

  int flangeMat;                    ///< Pipe material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void applyActiveFrontBack();
  
 public:

  MultiPipe(const std::string&);
  MultiPipe(const MultiPipe&);
  MultiPipe& operator=(const MultiPipe&);
  virtual ~MultiPipe();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
