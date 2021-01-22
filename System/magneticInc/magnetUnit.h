/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaMagneticInc/magnetUnit.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef magnetSystem_magnetUnit_h
#define magnetSystem_magnetUnit_h

namespace magnetSystem
{
  
/*!
  \class magnetUnit
  \version 1.0
  \date February 2019
  \author S. Ansell
  \brief Holds an external magnetic system

  FixedRotate holds the basis set for the axial components
*/

class magnetUnit :
  public attachSystem::FixedRotate
{
 protected:

  bool zeroField;         ///< If field currently zero
  size_t index;           ///< Order index for magnetic field

  Geometry::Vec3D magExtent;  ///< extent of magnetic field

  std::array<double,4> KFactor;  ///< Magnet units
  /// active cells
  std::set<int> activeCells; 

  void populate(const FuncDataBase&);

  void rotateExtent();
  
 public:

  magnetUnit(const std::string&);
  magnetUnit(const std::string&,const size_t);
  magnetUnit(const magnetUnit&);
  magnetUnit& operator=(const magnetUnit&);
  virtual ~magnetUnit();

  void setIndex(const size_t I) { index=I; }
  void setExtent(const double,const double,const double);
  void addCell(const int);
  void setKFactor(const std::vector<double>&);
  
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
