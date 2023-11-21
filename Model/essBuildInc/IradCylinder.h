/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/IradCylinder.h
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

class IradCylinder :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap
{
 private:
  
  int sampleActive;               ///< samples are not built if set to zero
  
  double radius;                  ///< Main radius [from built object]
  double length;                  ///< Length of cylinder
  std::vector<double> wallThick;  ///< Wall thicknesses

  double temp;                    ///< Temperature
  int mat;                        ///< Main material
  std::vector<int> wallMat;       ///< Wall material

  double sampleX;                 ///< Sample X step     
  double sampleY;                 ///< Sample Z step     
  double sampleZ;                 ///< Sample Z step
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createInnerObjects(Simulation&);
  void createLinks();

 public:

  IradCylinder(const std::string&);
  IradCylinder(const IradCylinder&);
  IradCylinder& operator=(const IradCylinder&);
  virtual IradCylinder* clone() const;
  ~IradCylinder() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
