/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/InnerZone.h
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
  \date October 2018
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
  attachSystem::FixedComp* FCPtr;      ///< Point to FixedComp for object
  attachSystem::CellMap* CellPtr;      ///< Pointer to cell map 

  std::vector<int> insertCN;            ///< InsertCell numbers
  std::map<int,HeadRule> insertCells;   ///< InsertCell BEFORE insertion
  
  HeadRule extraHR;
  HeadRule surroundHR;               ///< Rule of surround
  HeadRule frontHR;                  ///< Rule of front
  HeadRule backHR;                   ///< Rule of back
  HeadRule middleHR;                 ///< Rule of middle 

  HeadRule frontDivider;             ///< Local front divider [if needed]

  int voidMat;                       ///< Void material

  MonteCarlo::Object* masterCell;    ///< Current end cell
  
 public:

  InnerZone(attachSystem::FixedComp&,int&);
  InnerZone(const InnerZone&);
  InnerZone(InnerZone&&);
  InnerZone& operator=(const InnerZone&);
  ~InnerZone() {}         ///< Destructor

  void setExtra();
  void removeExtra() { extraHR.reset(); };
  void setSurround(const HeadRule&);
  void addMiddleToSurround(const int);
  void setFront(const HeadRule&);
  void setBack(const HeadRule&);
  void setMiddle(const HeadRule&);

  /// set the void material
  void setInnerMat(const int M) { voidMat=M; }
  
  InnerZone buildMiddleZone(const int) const;
  
  void constructMiddleSurface(ModelSupport::surfRegister&,
			      const int,const attachSystem::FixedComp&,
			      const long int);
  void constructMiddleSurface(ModelSupport::surfRegister&,const int,
			      const attachSystem::FixedComp&,const long int,
			      const attachSystem::FixedComp&,const long int);

  // split leaving middle section
  int triVoidUnit(Simulation&, MonteCarlo::Object*,
		  HeadRule&,const HeadRule&,const HeadRule&);

  int triVoidUnit(Simulation&, MonteCarlo::Object*,
		  const HeadRule&,const HeadRule&);

  // split deleting middle section
  int cutVoidUnit(Simulation&, MonteCarlo::Object*,
		  HeadRule&,const HeadRule&,const HeadRule&);

  int cutVoidUnit(Simulation&, MonteCarlo::Object*,
		  const HeadRule&,const HeadRule&);

  // chop leaving only first part:
  int singleVoidUnit(Simulation&, MonteCarlo::Object*,
		     HeadRule&,const HeadRule&);

  int singleVoidUnit(Simulation&, MonteCarlo::Object*,
		     const HeadRule&);


  int createFinalVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  const attachSystem::FixedComp&,
			  const long int);

  
  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  HeadRule&,
			  const attachSystem::FixedComp&,
			  const long int);
  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  const attachSystem::FixedComp&,
			  const long int);
  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  const attachSystem::FixedComp&,
			  const std::string&);

  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  const HeadRule&);

  int createOuterVoidUnit(Simulation&,
			  MonteCarlo::Object*,
			  HeadRule&,
			  const HeadRule&);


  int createNamedOuterVoidUnit(Simulation&,const std::string&,
			  MonteCarlo::Object*,
			  HeadRule&,
			  const attachSystem::FixedComp&,
			  const long int);
  
  int createNamedOuterVoidUnit(Simulation&,const std::string&,
			       MonteCarlo::Object*,
			       const attachSystem::FixedComp&,
			       const long int);
  int createNamedOuterVoidUnit(Simulation&,const std::string&,
			       MonteCarlo::Object*,
			       const HeadRule&);
  int createNamedOuterVoidUnit(Simulation&,const std::string&,
			       MonteCarlo::Object*,
			       HeadRule&,
			       const HeadRule&);
  
  void refrontMasterCell(MonteCarlo::Object*,
			 const HeadRule&) const;
  void cutMasterCell(MonteCarlo::Object*,
		     const HeadRule&,
		     const HeadRule&) const;

  void setInsertCells(const std::vector<int>&);
  
  MonteCarlo::Object* constructMasterCell(Simulation&);
  MonteCarlo::Object* constructMasterCell(Simulation&,
					  const attachSystem::FixedComp&,
					  const long int);
  MonteCarlo::Object* constructMasterCell(Simulation&,
					  const attachSystem::FixedComp&,
					  const std::string&);
  
  
  void removeLastMaster(Simulation&);
  
  /// accessor to local master cell
  MonteCarlo::Object* getMaster() const { return masterCell; }
  
};

}

#endif
 
