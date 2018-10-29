/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/InnerZone.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef attachSystem_InnerZone_h
#define attachSystem_InnerZone_h

class Simulation;

namespace Geometry
{
  class Line;
}

namespace attachSystem
{
  class FixedComp;
  class CellMap;
  
/*!
  \class InnerZone
  \version 1.0
  \author S. Ansell
  \date October 2019
  \brief Builds an inner unit within an object

  This object is for objects that contain an inner space.
  The FCPtr and CellPtr need to point to the possessing object
  so that they don't become obsoleted as the shared_ptr is deleted.

*/

class InnerZone  
{
 private:

  std::string FCName;                  ///< Fixed comp name [if available]
  int& cellIndex;                      ///< Cell index from FCPtr
  attachSystem::FixedComp* FCPtr;      ///< Point to FixedComp [
  attachSystem::CellMap* CellPtr;      ///< Pointer to cell map 

  HeadRule surroundHR;               ///< Rule of surround
  HeadRule frontHR;                  ///< Rule of back
  HeadRule backHR;                   ///< Rule of back
  HeadRule middleHR;                 ///< Rule of middel 

  HeadRule frontDivider;             ///< Local front divider [if needed]
  
 public:


  InnerZone(attachSystem::FixedComp&,int&);
  InnerZone(const InnerZone&);
  InnerZone& operator=(const InnerZone&);
  ~InnerZone() {}


  void setSurround(const HeadRule&);
  void setFront(const HeadRule&);
  void setBack(const HeadRule&);
  void setMiddle(const HeadRule&);

  void constructMiddleSurface(ModelSupport::surfRegister&,
			      const int,const attachSystem::FixedComp&,
			      const long int);

  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object&,
			  HeadRule&,
			  const attachSystem::FixedComp&,
			  const long int);
  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object&,
			  const attachSystem::FixedComp&,
			  const long int);

  
  std::pair<int,int> createOuterVoidPair(Simulation&,
					 MonteCarlo::Object&,
					 const attachSystem::FixedComp&,
					 const long int);

  std::pair<int,int> createOuterVoidPair(Simulation&,
					 MonteCarlo::Object&,
					 HeadRule&,
					 const attachSystem::FixedComp&,
					 const long int);
  

  void refrontMasterCell(MonteCarlo::Object&,
			 const attachSystem::FixedComp&,
			 const long int) const;

  void refrontMasterCell(MonteCarlo::Object&,
			 MonteCarlo::Object&,
			 const attachSystem::FixedComp&,
			 const long int) const;

  MonteCarlo::Object& constructMasterCell(Simulation&);
  MonteCarlo::Object& constructMasterCell
    (Simulation&,const ContainedComp&);
  
};

}

#endif
 
