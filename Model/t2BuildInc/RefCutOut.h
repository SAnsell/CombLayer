/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/RefCutOut.h
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
#ifndef moderatorSystem_RefCutOut_h
#define moderatorSystem_RefCutOut_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class RefCutOut
  \version 1.0
  \author S. Ansell
  \date August 2011
  \brief Reflector cut for ChipIR
*/

class RefCutOut : 
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
  
{
 private:

  double tarOutStep;           ///< Distance from target centre
  double radius;               ///< radius of hole
  
  int mat;                     ///< Material
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);

 public:

  RefCutOut(const std::string&);
  RefCutOut(const RefCutOut&);
  RefCutOut& operator=(const RefCutOut&);
  ~RefCutOut();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
