/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/FuelElement.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef delftSystem_FuelElement_h
#define delftSystem_FuelElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class FuelElement
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Creates a fuel element 
*/

class FuelElement  : public RElement
{
 protected:

  size_t nElement;           ///< Number of elements

  double depth;              ///< Total depth of the cell
  double width;              ///< Total width of the cell
  double topHeight;          ///< Total depth of the cell
  double baseHeight;         ///< Total depth of the cell
  double baseRadius;         ///< Radius 
  double fuelHeight;         ///< Fuel Height (Z)
  double fuelWidth;          ///< Fuel Width (X)
  double fuelDepth;          ///< Fuel Depth (Y)
  double cladHeight;         ///< Cladding thickness [one side]
  double cladDepth;          ///< Cladding Depth [one side]
  double cladWidth;          ///< Cladding extra with + fuel
  double waterDepth;         ///< Cladding extra with + fuel

  double barRadius;          ///< Handle radius
  double barOffset;          ///< Handle vertical offset

  int alMat;                 ///< al material
  int watMat;                 ///< al material
  int fuelMat;                 ///< al material

  // THIS MUST BE IN A CLASS
  size_t nFuel;
  std::vector<int> fuelCells;   ///< Cells with U for division
  std::vector<double> fuelFrac;  ///< divider of fuel
  std::vector<int> fMat;

  std::vector<int> midCell;                  ///< Mid cell if needed
  std::vector<Geometry::Vec3D> midCentre;    ///< Mid centre
  int topCell;                  ///< Mid cell if needed

  void populate(const Simulation&);
  void createUnitVector(const FixedComp&,const Geometry::Vec3D&);
  
  void createSurfaces(const attachSystem::FixedComp&,
		      const size_t,const size_t);
  void createSurfaces(const attachSystem::FixedComp&,
		      const std::set<size_t>&);
  void createObjects(Simulation&,const size_t,const size_t);
  void createObjects(Simulation&,const std::set<size_t>&);;
  void createLinks();

 
  void layerProcess(Simulation&); 
  
  Geometry::Vec3D plateCentre(const size_t) const;

 public:

  FuelElement(const size_t,const size_t,const std::string&);
  FuelElement(const FuelElement&);
  FuelElement& operator=(const FuelElement&);
  virtual ~FuelElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const FixedComp&,const Geometry::Vec3D&);

};

}  

#endif
