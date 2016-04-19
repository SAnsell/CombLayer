/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/IradCylinder.h
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
#ifndef essSystem_IradCylinder_h
#define essSystem_IradCylinder_h

class Simulation;

namespace essSystem
{
/*!
  \class IradCylinder
  \author S. Ansell
  \version 1.1
  \date April 2016
  \brief Iradiation object within the Pre-mod
*/

class IradCylinder : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int iradIndex;            ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double radius;                  ///< Main radius [from built object]
  double length;                  ///< Length of cylinder
  double wallThick;               ///< Wall thickness

  double temp;                    ///< Temperature
  int mat;                        ///< Main material
  int wallMat;                    ///< Wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  IradCylinder(const std::string&);
  IradCylinder(const IradCylinder&);
  IradCylinder& operator=(const IradCylinder&);
  virtual IradCylinder* clone() const;
  virtual ~IradCylinder();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
