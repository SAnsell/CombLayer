/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/simpleObj.h
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
#ifndef testSystem_simpleObj_h
#define testSystem_simpleObj_h

class Simulation;

/*!
  \namespace testSystem
  \version 1.0
  \author S. Ansell
  \date September 2012
  \brief Simple test framework
  
  This is a test namespace for objects that dont appear in 
  the main systems.
*/

namespace testSystem
{

/*!
  \class simpleObj
  \version 1.0
  \author S. Ansell
  \date September 2012
  \brief Simple Box object for testing
*/

class simpleObj :
    public attachSystem::ContainedComp,
    public attachSystem::FixedComp,
    public attachSystem::CellMap
{
 private:
  
  int refFlag;                  ///< Reverse flag
  Geometry::Vec3D offset;       ///< Central offset
  double xyAngle;               ///< Angle [degrees]
  double zAngle;                ///< Angle [degrees]
  double xSize;                 ///< Left/Right size
  double ySize;                 ///< length down target
  double zSize;                 ///< Vertical size

  int defMat;                   ///< Default material  

  using FixedComp::createUnitVector;
  void createUnitVector(const attachSystem::FixedComp&,const long int) override;
  void createUnitVector(const attachSystem::FixedComp&,const double,
			const double,const double);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  simpleObj(const std::string&);
  simpleObj(const simpleObj&);
  simpleObj& operator=(const simpleObj&);
  ~simpleObj() override;
    
  /// Set Material
  void setMat(const int M) { defMat=M; }
  void setRefFlag(const int R) {refFlag=R;}
  /// Set offset
  void setOffset(const Geometry::Vec3D& O) { offset=O; }

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const double,const double,const double);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
