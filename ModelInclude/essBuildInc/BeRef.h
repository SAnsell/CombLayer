/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/BeRef.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef essSystem_BeRef_h
#define essSystem_BeRef_h

class Simulation;

namespace essSystem
{

/*!
  \class BeRef
  \author S. Ansell
  \version 1.0
  \date February 2013
  \brief Reflector object 
*/

class BeRef : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int refIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  double radius;                  ///< Radius
  double height;                  ///< Height
  double wallThick;               ///< Wall thickness

  double targSepThick;            ///< Steel seperator at target level

  int refMat;                     ///< reflector material
  int wallMat;                    ///< wall Material
  int targSepMat;                 ///< Separator Mat

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BeRef(const std::string&);
  BeRef(const BeRef&);
  BeRef& operator=(const BeRef&);
  virtual ~BeRef();

  void createAll(Simulation&,const attachSystem::FixedComp&);
  
};

}

#endif
 
