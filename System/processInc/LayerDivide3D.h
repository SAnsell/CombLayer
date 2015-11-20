/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/LayerDivide3D.h
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
#ifndef ModelSupport_LayerDivide3D_h
#define ModelSupport_LayerDivide3D_h

class Simulation;

namespace ModelSupport
{
  class DivideGrid;
  
/*!
  \class LayerDivide3D
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Bulk divider to produce 3D grid in a cell
*/

class LayerDivide3D 
{
 private:

  const std::string keyName;       ///< Key name
  ModelSupport::surfRegister SMap; ///< Surface register
  Geometry::Vec3D Centre;          ///< Centre point
  
  const int divIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  size_t nALen;                  ///< Number of sector divisions
  std::vector<double> ALen;      ///< Thickness in A direction

  size_t nBLen;                  ///< Number of vertical divisions
  std::vector<double> BLen;      ///< Thickness of B direction

  size_t nCLen;                  ///< number layer in C direction
  std::vector<double> CLen;      ///< thicknesss (fractions)

  std::pair<int,int> AWall;      ///< A wall surf numbers
  std::pair<int,int> BWall;      ///< B wall surf numbers
  std::pair<int,int> CWall;      ///< C wall surf numbers
  
  DivideGrid* DGPtr;             ///< Main divider [if used]

  std::string objName;           ///< XML component name
  std::string loadFile;          ///< File to load
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const attachSystem::FixedComp&,
			const long int,const bool);

  

  void addCalcPoint(const size_t,const size_t,const size_t,
		    std::string);


  void processSurface(const size_t,
		      const std::pair<int,int>&,
		      const std::vector<double>&);
  
 public:

  LayerDivide3D(const std::string&);
  LayerDivide3D(const LayerDivide3D&);
  LayerDivide3D& operator=(const LayerDivide3D&);
  virtual ~LayerDivide3D();

  void setSurfPair(const size_t,const int,const int);

  void divideCell(Simulation&,const int);
    
};

}

#endif
 

