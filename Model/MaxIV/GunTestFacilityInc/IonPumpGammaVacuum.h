/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/IonPumpGammaVacuum.h
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
#ifndef MAXIV_IonPumpGammaVacuum_h
#define MAXIV_IonPumpGammaVacuum_h

class Simulation;

namespace MAXIV
{

/*!
  \class IonPumpGammaVacuum
  \version 1.0
  \author Konstantin Batkov
  \date September 2023
  \brief Ion Pump made by Gamma Vacuum
*/

class IonPumpGammaVacuum :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Body + wall length (excluding flange)
  double height;                ///< Height
  double wallThick;             ///< Wall thickness
  double zClearance;            ///< z-axis clearance
  double pistonWidth;           ///< Piston width
  double pistonHeight;          ///< Piston height
  double pistonBaseHeight;      ///< Piston base height
  double pistonBaseThick;       ///< Piston base thick
  double pistonLength;          ///< Piston total length (including the base)
  double flangeRadius;          ///< Flange connector radius
  double flangeThick;           ///< Flange cup thick
  double flangeTubeRadius;      ///< Flange tube inner radius
  double flangeTubeThick;       ///< Flange tube thick
  double flangeTubeLength;      ///< Flange tube total length

  int mainMat;                  ///< Main material
  int wallMat;                  ///< Wall material
  int pistonMat;                ///< Piston material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  IonPumpGammaVacuum(const std::string&);
  IonPumpGammaVacuum(const IonPumpGammaVacuum&);
  IonPumpGammaVacuum& operator=(const IonPumpGammaVacuum&);
  virtual IonPumpGammaVacuum* clone() const;
  virtual ~IonPumpGammaVacuum();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
