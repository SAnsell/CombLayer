/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   magneticInc/elecUnit.h
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
#ifndef magnetSystem_elecUnit_h
#define magnetSystem_elecUnit_h

namespace magnetSystem
{
  
/*!
  \class elecUnit
  \version 1.0
  \date May 2022
  \author S. Ansell
  \brief Holds an variable electric field (RF)

  This component holds a variable electric field for
  R.F components
*/

class elecUnit :
  public attachSystem::FixedRotate
{
 protected:

  bool zeroField;         ///< If field currently zero
  size_t index;           ///< ID number for electric field
  
  Geometry::Vec3D extent;  ///< extent of electric field

  /// active cells
  std::set<int> activeCells; 

  void populate(const FuncDataBase&);

  void rotateExtent();
  
 public:

  elecUnit(const std::string&);
  elecUnit(const std::string&,const size_t);
  elecUnit(const elecUnit&);
  elecUnit& operator=(const elecUnit&);
  virtual ~elecUnit();

  void setIndex(const size_t I) { index=I; }
  void setExtent(const double,const double,const double);
  void addCell(const int);
  
  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(Simulation&,const Geometry::Vec3D&, 
   		 const Geometry::Vec3D&,const Geometry::Vec3D&,
   		 const Geometry::Vec3D&, 
   		 const std::vector<double>&);


  
  virtual void writePHITS(std::ostream&) const;
  virtual void writeFLUKA(std::ostream&) const;
};

}

#endif
