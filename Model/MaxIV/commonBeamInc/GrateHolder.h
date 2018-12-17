/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/GrateHolder.h
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
#ifndef xraySystem_GrateHolder_h
#define xraySystem_GrateHolder_h

class Simulation;

namespace xraySystem
{

/*!
  \class GrateHolder
  \author S. Ansell
  \version 1.0
  \date December 2018
  \brief Grating and holder
*/

class GrateHolder :
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  int indexPoint;           ///< Extra X step for index shift
  double fullLength;        ///< Length + ends
    
  double gWidth;            ///< Radius of from centre
  double gThick;            ///< Radius of detector
  double gLength;           ///< Outer wall thickness

  
  double sideThick;         ///< Width of sides
  double sideHeight;        ///< height of side

  double endThick;           ///< end thick (y)
  double endWidth;           ///< end widht (x)
  double endHeight;         ///< end length

  int xstalMat;           ///< slide material
  int mainMat;            ///< Main metal
  int sideMat;            ///< slide material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  GrateHolder(const std::string&);
  GrateHolder(const GrateHolder&);
  GrateHolder& operator=(const GrateHolder&);
  virtual ~GrateHolder();

  /// Index Position
  void setIndexPosition(const int I) { indexPoint=I; }
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
