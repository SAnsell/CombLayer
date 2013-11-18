/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/ShutterBay.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef essSystem_ShutterBay_h
#define essSystem_ShutterBay_h

class Simulation;

namespace essSystem
{

/*!
  \class Reflector
  \version 1.0
  \author S. Ansell
  \date November 2012
  \brief Reflector Cylindrical
*/

class ShutterBay : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int bulkIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double xStep;                 ///< Offset on X to Target
  double yStep;                 ///< Offset on Y to Target [+ve forward]
  double zStep;                 ///< Offset on Z top Target

  double xyAngle;               ///< Angle of master XY rotation
  double zAngle;                ///< Angle of master Z rotation

  double radius;        ///< radius of outer
  double height;        ///< height
  double depth;         ///< Depth 
  int mat;              ///< Material

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::ContainedComp&);

 public:

  ShutterBay(const std::string&);
  ShutterBay(const ShutterBay&);
  ShutterBay& operator=(const ShutterBay&);
  virtual ~ShutterBay();

  /// Access to main cell
  int getMainCell() const { return cellIndex-1; }

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&);

};

}

#endif
 

