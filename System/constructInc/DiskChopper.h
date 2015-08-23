/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/DiskChopper.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef constructSystem_DiskChopper_h
#define constructSystem_DiskChopper_h

class Simulation;

namespace constructSystem
{
  class DiskBlades;
/*!
  \class DiskChopper
  \version 1.0
  \author S. Ansell
  \date January 2015
  \brief DiskChopper unit  
*/

class DiskChopper : public attachSystem::FixedGroup,
    public attachSystem::ContainedComp
{
 private:
  
  const int chpIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int centreFlag;               ///< Centre origin / edge origin
  
  double xStep;                 ///< Origin step
  double yStep;                 ///< Origin step
  double zStep;                 ///< Origin step
  double xyAngle;               ///< Axis rotation
  double zAngle;                ///< Axis rotation
  
  double innerRadius;           ///< Inner Non-Viewed radius 
  double outerRadius;           ///< Outer Viewed radius
  double diskGap;               ///< Gap betwen disks

  size_t nDisk;                 ///< Number of disks
  std::vector<DiskBlades> DInfo;  ///< Info on each disk

  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createUnitVector(const attachSystem::TwinComp&,const long int);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DiskChopper(const std::string&);
  DiskChopper(const DiskChopper&);
  DiskChopper& operator=(const DiskChopper&);
  virtual ~DiskChopper();


  /// Access centre flag
  void setCentreFlag(const int C) { centreFlag=C; }
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAllBeam(Simulation&,const attachSystem::TwinComp&,
		     const long int);

};

}

#endif
 
