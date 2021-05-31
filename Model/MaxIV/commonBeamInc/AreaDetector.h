/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxsInc/AreaDetector.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef xraySystem_AreaDetector_h
#define xraySystem_AreaDetector_h

class Simulation;

namespace xraySystem
{

/*!
  \class AreaDetector
  \version 1.0
  \author Konstantin Batkov
  \date 25 Jun 2019
  \brief Wide-angle X-ray scattering (WAXS) detector
*/

class AreaDetector :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  double length;                ///< Total length
  double width;                 ///< Width
  double height;                ///< Height
  double screenThick;           ///< Wall thickness
  double screenDepth;           ///< screen depth from front
  double wallThick;             ///< Wall thickness

  int voidMat;                  ///< Void (front) material
  int detMat;                   ///< Main detector material
  int screenMat;                ///< Screen material
  int wallMat;                  ///< Wall material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  AreaDetector(const std::string&);
  AreaDetector(const AreaDetector&);
  AreaDetector& operator=(const AreaDetector&);
  virtual ~AreaDetector();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


