/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/Solenoid.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef xraySystem_Solenoid_h
#define xraySystem_Solenoid_h

class Simulation;

namespace xraySystem
{

/*!
  \class Solenoid
  \version 1.0
  \author Konstantin Batkov
  \date November 2023
  \brief Emittance compensating Solenoid
*/

class Solenoid :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double frameWidth;            ///< Frame width
  double frameThick;            ///< Frame thickness (outer walls)
  double coilThick;             ///< Single wire coil thickness
  double coilRadius;            ///< Wire coil radius
  double coilGap;               ///< Radial gap between coil and frame
  double penRadius;             ///< Axial penetration radius

  int frameMat;                 ///< Frame material
  int coilMat;                  ///< Coil material
  int voidMat;                  ///< Void material
  size_t nCoils;                ///< Number of coils
  size_t nFrameFacets;          ///< Number of frame facets

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Solenoid(const std::string&);
  Solenoid(const Solenoid&);
  Solenoid& operator=(const Solenoid&);
  virtual Solenoid* clone() const;
  virtual ~Solenoid();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
