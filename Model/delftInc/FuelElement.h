/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/FuelElement.h
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

  std::vector<Geometry::Vec3D> fuelCentre;    ///< Centre with U 
  std::vector<double> fuelFrac;     ///< divider of fuel

  std::vector<Geometry::Vec3D> midCentre;    ///< Mid centre

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);;
  void createLinks();

 
  void layerProcess(Simulation&,const FuelLoad&); 
  
  Geometry::Vec3D plateCentre(const size_t) const;

  void makeFuelDivider();
  void addWaterExclude(Simulation&,const Geometry::Vec3D&,
		       const HeadRule&);

 public:

  FuelElement(const size_t,const size_t,const std::string&);
  FuelElement(const FuelElement&);
  FuelElement& operator=(const FuelElement&);
  ~FuelElement() override {}   ///< Destructor

  bool isFuel(const size_t) const;
  /// Accessor to fuel material
  int getDefMat() const { return fuelMat; } 
  /// Accessor to number blades
  size_t getNElements() const { return nElement; }
  /// Accessor to number cells in a blade
  size_t getNSections() const { return nFuel; }

  void setExcludeRange(const size_t,const size_t);
  /// Exclude set
  const std::set<size_t>& getRemovedSet() const { return Exclude; } 
  /// Access centres [for source]
  const std::vector<Geometry::Vec3D>& getFuelCentre() const 
     { return fuelCentre; } 

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}  

#endif
