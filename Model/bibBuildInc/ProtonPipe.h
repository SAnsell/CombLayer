/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/ProtonPipe.h
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
#ifndef bibSystem_ProtonPipe_h
#define bibSystem_ProtonPipe_h

class Simulation;

namespace bibSystem
{

/*!
  \class ProtonPipe
  \author S. Ansell
  \version 1.0
  \date April 2013
  \brief Specialized proton input for wheel
*/

class ProtonPipe : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int protonIndex;          ///< Index of surface offset
  int cellIndex;                  ///< Cell index


  double radius;                  ///< Radius of inner pipe
  double innerWallThick;          ///< Inner wall thickness
  double wallThick;               ///< Main wall thickness
  
  double length;                  ///< Full Length of pipe
  double innerLength;             ///< Partial Length of pipe

  int voidMat;                 ///< Inner void material
  int innerWallMat;            ///< Inner Wall material
  int wallMat;                 ///< Wall material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
		  const long int);

  void createSurfaces(const attachSystem::FixedComp&,
		     const long int);
  void createObjects(Simulation&);
  void createLinks();

  public:

  ProtonPipe(const std::string&);
  ProtonPipe(const ProtonPipe&);
  ProtonPipe& operator=(const ProtonPipe&);
  virtual ~ProtonPipe();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);
  
};

}

#endif
 
