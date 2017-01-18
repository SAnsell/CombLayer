/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/CrystalMount.h
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
#ifndef constructSystem_CrystalMount_h
#define constructSystem_CrystalMount_h

class Simulation;

namespace constructSystem
{

/*!
  \class CrystalMount
  \author S. Ansell
  \version 1.0
  \date January 2013
  \brief Crystal in holder with linkpoint on reflection axis
*/

class CrystalMount : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  const std::string baseName;  ///< Base name
  const size_t ID;             ///< ID Number
  
  const int xtalIndex;     ///< Index of surface offset
  int cellIndex;           ///< Cell index

  double width;            ///< Radius of from centre
  double thick;            ///< Radius of detector
  double length;           ///< Outer wall thickness

  double gap;              ///< Gap thickness
  double wallThick;        ///< Wall thickness
  double baseThick;        ///< Base thickness

  int xtalMat;             ///< XStal material
  int wallMat;             ///< Wall material

  Geometry::Vec3D viewPoint;  // Origin point
  
  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CrystalMount(const std::string&,const size_t);
  CrystalMount(const CrystalMount&);
  CrystalMount& operator=(const CrystalMount&);
  virtual ~CrystalMount();

  void createTally(Simulation&) const;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
