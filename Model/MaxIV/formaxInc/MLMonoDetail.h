/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formaxInc/MLMonoDetail.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef xraySystem_MLMonoDetail_h
#define xraySystem_MLMonoDetail_h

class Simulation;

namespace xraySystem
{

  class MLMCrystal;
  
/*!
  \class MLMonoDetail
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Double Mirror Mono arrangement
*/

class MLMonoDetail :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  std::shared_ptr<MLMCrystal> xstalA;
  std::shared_ptr<MLMCrystal> xstalB;
  
  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMonoDetail(const std::string&);
  MLMonoDetail(const MLMonoDetail&);
  MLMonoDetail& operator=(const MLMonoDetail&);
  virtual ~MLMonoDetail();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
