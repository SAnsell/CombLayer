/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/Bunker.h
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
#ifndef essSystem_Bunker_h
#define essSystem_Bunker_h

class Simulation;

namespace essSystem
{

/*!
  \class Bunker
  \version 1.0
  \author S. Ansell
  \date April 2013
  \brief Bulk around Reflector
*/

class Bunker : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int bulkIndex;           ///< Index of surface offset
  int cellIndex;                 ///< Cell index

  double leftPhase;              ///< Sector phase left
  double rightPhase;             ///< Sector phase rightx
  
  double leftAngle;              ///< Extent of left angle
  double rightAngle;             ///< Extent of right ange

  double floorDepth;             ///< 
  double roofDepth;

  double wallThick;
  double roofThick;
  double floorThick;

  int mat;

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);

 public:

  Bunker(const std::string&);
  Bunker(const Bunker&);
  Bunker& operator=(const Bunker&);
  virtual ~Bunker();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 

