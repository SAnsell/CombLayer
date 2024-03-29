/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/LayerDivide1D.h
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
#ifndef ModelSupport_LayerDivide1D_h
#define ModelSupport_LayerDivide1D_h

class Simulation;

namespace ModelSupport
{
/*!
  \class LayerDivide1D
  \version 1.0
  \author S. Ansell
  \date November 2015
  \brief Bulk divider to produce 1D grid in a cell
*/

class LayerDivide1D  : public attachSystem::FixedUnit,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  Geometry::Vec3D Centre;        ///< Centre point
  

  std::vector<double> AFrac;     ///< Fractions in A direction
  std::vector<int> AMat;         ///< Materials  

  size_t ALen;                   ///< Number of A Segments

  std::string WallID;            ///< ID names for walls
  std::pair<int,int> AWall;      ///< A wall surf numbers
  HeadRule dividerHR;           ///< divider string [if any]

  std::string objName;           ///< XML component name
  std::string loadFile;          ///< File to load
  std::string outputFile;        ///< File to write
  
  void populate(const FuncDataBase&);

  
  void checkDivide() const;
  
  void addCalcPoint(const size_t,const size_t,const size_t);


  size_t processSurface(const std::pair<int,int>&,
			const std::vector<double>&);
  
 public:

  LayerDivide1D(const std::string&);
  LayerDivide1D(const LayerDivide1D&);
  LayerDivide1D& operator=(const LayerDivide1D&);
  ~LayerDivide1D() override;

  void setSurfPair(const int,const int);
  void setFractions(const std::vector<double>&);
  void setFractions(const size_t);

  void setIndexNames(const std::string&);
  void setDivider(const HeadRule&);

  void setMaterials(const std::string&);
  void setMaterials(const std::vector<std::string>&);
  void setMaterials(const std::vector<int>&);
  int setMaterialXML(const std::string&,const std::string&,
		      const std::string&,const std::string&);
  
  void divideCell(Simulation&,const int);
    
};

}

#endif
 

