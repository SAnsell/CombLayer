/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/FixedMaskHybrid.h
 *
 * Copyright (c) 2004-2025 by Konstantin Batkov
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
#ifndef xraySystem_FixedMaskHybrid_h
#define xraySystem_FixedMaskHybrid_h

class Simulation;

namespace xraySystem
{

/*!
  \class FixedMaskHybrid
  \version 1.0
  \author Konstantin Batkov
  \date December 2025
  \brief Hybrid Fixed Mask for Toyama front-end at DanMAX. Main drawing: S1-2-2AG00580.pdf
         Outer shape: cylindrical
	 Entrance mask: cone
	 Exit mask: rectangle
*/

class FixedMaskHybrid :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length
  double radius;                ///< Outer radius
  double flangeLength;          ///< Flange length
  double flangeRadius;          ///< Flange radius

  int mat;                      ///< FM material
  int flangeMat;                ///< Flange material
  int voidMat;                  ///< Void material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FixedMaskHybrid(const std::string&);
  FixedMaskHybrid(const FixedMaskHybrid&);
  FixedMaskHybrid& operator=(const FixedMaskHybrid&);
  virtual FixedMaskHybrid* clone() const;
  virtual ~FixedMaskHybrid();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
