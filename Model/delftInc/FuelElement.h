/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/FuelElement.h
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

  size_t nElement;            ///< Number of elements
  std::set<size_t> Exclude;   ///< Exclude set

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
  int watMat;                ///< coollant material
  int fuelMat;               ///< default fuel material

  size_t nFuel;                     ///< Number of fuel sub-cells in a strip
  std::vector<int> fuelCells;       ///< Cells with U for division
  std::vector<Geometry::Vec3D> fuelCentre;    ///< Centre with U 
  std::vector<int> waterCells;      ///< Cells with H2O coolant [for insertion]
  std::vector<double> fuelFrac;     ///< divider of fuel

  std::vector<int> midCell;                  ///< Mid cell if needed
  std::vector<Geometry::Vec3D> midCentre;    ///< Mid centre
  int topCell;                               ///< Mid cell if needed

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const Geometry::Vec3D&);
  
  void createSurfaces(const attachSystem::FixedComp&,
		      const size_t,const size_t);
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);;
  void createLinks();

 
  void layerProcess(Simulation&,const FuelLoad&); 
  
  Geometry::Vec3D plateCentre(const size_t) const;

  void makeFuelDivider();
  void addWaterExclude(Simulation&,const Geometry::Vec3D&,
		       const std::string&);

 public:

  FuelElement(const size_t,const size_t,const std::string&);
  FuelElement(const FuelElement&);
  FuelElement& operator=(const FuelElement&);
  virtual ~FuelElement() {}   ///< Destructor

  bool isFuel(const size_t) const;
  /// Accessor to fuel material
  int getDefMat() const { return fuelMat; } 
  /// Accessor to number blades
  size_t getNElements() const { return nElement; }
  /// Accessor to number cells in a blade
  size_t getNSections() const { return nFuel; }

  /// Exclude set
  const std::set<size_t>& getRemovedSet() const { return Exclude; } 
  /// Fuel Vector
  const std::vector<int>& getFuel() { return fuelCells; } 
  /// Access centres [for source]
  const std::vector<Geometry::Vec3D>& getFuelCentre() const 
     { return fuelCentre; } 
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const Geometry::Vec3D&,
			 const FuelLoad&);

};

}  

#endif
