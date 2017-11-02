/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delftInc/ReactorGrid.h
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
#ifndef delftSystem_ReactorGrid_h
#define delftSystem_ReactorGrid_h

class Simulation;

namespace delftSystem
{
  class RElement;
  class beamSlot;
  class BeamInsert;
  class BeamTube;
  class FuelLoad;

/*!
  \class ReactorGrid
  \version 1.0
  \author S. Ansell
  \date June 2012

  \brief Grid plate for Delft core elements
*/

class ReactorGrid : public attachSystem::FixedOffset,
    public attachSystem::ContainedComp
{
 private:
  
  /// reactor element storeage type
  typedef std::shared_ptr<RElement> RTYPE;

  const int gridIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  size_t NX;                    ///< across size
  size_t NY;                    ///< tube direction size
  
  double Width;                 ///< Full X Width
  double Depth;                 ///< Full Y Depth
  double Base;                  ///< Full Z Base depth
  double Top;                   ///< Full Z Height
  double plateThick;            ///< Plate thickness
  double plateRadius;           ///< Plate Radius
  int plateMat;                 ///< plate material
  int waterMat;                 ///< Water material in plate

  FuelLoad FuelSystem;          ///< System of fuel layout
  boost::multi_array<std::string,2> GType;      ///< Grid type
  boost::multi_array<RTYPE,2> Grid;     ///< Storage of the grid [size 3]

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  int getCellNumber(const long int,const long int) const;

 public:

  ReactorGrid(const std::string&);
  ReactorGrid(const ReactorGrid&);
  ReactorGrid& operator=(const ReactorGrid&);
  virtual ~ReactorGrid();

  void createElements(Simulation&);

  /// Size accessor
  std::pair<size_t,size_t> getSize() const 
    { return std::pair<size_t,size_t>(NX,NY); }
  Geometry::Vec3D getCellOrigin(const size_t,const size_t) const;
  // Helper for the fuel element
  template<typename T> 
  static T getElement(const FuncDataBase&,const std::string&,
		      const size_t,const size_t);
  template<typename T> 
  static T getDefElement(const FuncDataBase&,const std::string&,
			 const size_t,const size_t,const std::string&);

  static int getMatElement(const FuncDataBase&,const std::string&,
			   const size_t,const size_t);
  // Helper for the fuel element  
  static std::string getElementName(const std::string&,
				    const size_t,const size_t);
  static std::pair<size_t,size_t> getElementNumber(const std::string&);

  /// Access water material for inner objects
  int getWaterMat() const { return waterMat; }

  std::vector<Geometry::Vec3D> fuelCentres() const;
  std::vector<int> getAllCells(const Simulation&) const;
  std::vector<int> getFuelCells(const Simulation&,const size_t) const;
  

  void loadFuelXML(const std::string&);
  void writeFuelXML(const std::string&);

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);
    
};

}

#endif
 
