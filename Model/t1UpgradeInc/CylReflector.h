/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/CylReflector.h
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
#ifndef ts1System_CylReflector_h
#define ts1System_CylReflector_h

class Simulation;

namespace TMRSystem
{
  class TS2target;
  class TS2moly;
}

namespace ts1System
{

/*!
  \class CylReflector
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief Reflector Cylindrical
*/

class CylReflector : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  size_t nLayer;                 ///< Number of layers
  std::vector<double> radius;        ///< radius of outer
  std::vector<double> height;        ///< Heights
  std::vector<double> depth;         ///< Depths
  std::vector<Geometry::Vec3D> COffset;   ///< Centre offset

  std::vector<int> Mat;             ///< Default materials

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  CylReflector(const std::string&);
  CylReflector(const CylReflector&);
  CylReflector& operator=(const CylReflector&);
  virtual ~CylReflector();

  std::string getComposite(const std::string&) const;

  // Main cell
  int getInnerCell() const { return refIndex+1; }
  std::vector<int> getCells() const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
