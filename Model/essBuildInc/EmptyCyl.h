/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/EmptyCyl.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_EmptyCyl_h
#define essSystem_EmptyCyl_h

class Simulation;

namespace essSystem
{

/*!
  \class EmptyCyl
  \version 1.0
  \author Konstantin Batkov
  \date 23 Feb 2018
  \brief Empty area above Wheel
*/

class EmptyCyl : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double height;                ///< height
  int mat;                   ///< material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const long int,const long int,
		     const attachSystem::FixedComp&,const std::string);
  void createLinks(const attachSystem::FixedComp&,const long int,const long int);
  
 public:

  EmptyCyl(const std::string&);
  EmptyCyl(const EmptyCyl&);
  EmptyCyl& operator=(const EmptyCyl&);
  virtual EmptyCyl* clone() const;
  virtual ~EmptyCyl();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const long int,const long int,
		 const attachSystem::FixedComp&,const std::string);

};

}

#endif
 

