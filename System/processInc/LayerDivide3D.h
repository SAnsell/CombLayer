/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/LayerDivide3D.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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


class LayerDivide3D : public attachSystem::FixedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  Geometry::Vec3D Centre;          ///< Centre point
  
  const int divIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  std::vector<double> AFrac;     ///< Fractions in A direction
  std::vector<double> BFrac;     ///< Fractions of B direction
  std::vector<double> CFrac;     ///< Fractions in C direction 

  size_t ALen;                   ///< Number of A Segments
  size_t BLen;                   ///< Number of B Segments
  size_t CLen;                   ///< Number of C segments

  std::vector<std::string> WallID;  ///< ID names for each wall
  std::pair<int,int> AWall;      ///< A wall surf numbers
  std::pair<int,int> BWall;      ///< B wall surf numbers
  std::pair<int,int> CWall;      ///< C wall surf numbers
  std::string divider;           ///< divider string [if any]

  DivideGrid* DGPtr;             ///< Main divider materials

  std::string objName;           ///< XML component name
  std::string loadFile;          ///< File to load

  std::string outputFile;        ///< File to write
  
  void populate(const FuncDataBase&);

  
  void checkDivide() const;
  
  void addCalcPoint(const size_t,const size_t,const size_t);


  size_t processSurface(const size_t,
		     const std::pair<int,int>&,
		     const std::vector<double>&);
  
 public:

  LayerDivide3D(const std::string&);
  LayerDivide3D(const LayerDivide3D&);
  LayerDivide3D& operator=(const LayerDivide3D&);
  virtual ~LayerDivide3D();

  void setSurfPair(const size_t,const int,const int);
  void setFractions(const size_t,const std::vector<double>&);
  void setFractions(const size_t,const size_t);

  void setIndexNames(const std::string&,const std::string&,
		  const std::string&);
  void setDivider(const std::string&);
  void setDividerByExclude(const Simulation&,const int);

  void setMaterials(const std::string&);
  void setMaterials(const size_t,const std::vector<std::string>&);
  int setMaterialXML(const std::string&,const std::string&,
		     const std::string&,const std::string&);
  
  void divideCell(Simulation&,const int);
    
};

}

#endif
 

