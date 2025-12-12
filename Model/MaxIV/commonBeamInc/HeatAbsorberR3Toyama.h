/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/HeatAbsorberR3Toyama.h
 *
 * Copyright (c) 2025 by Udo Friman-Gayer
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
#ifndef xraySystem_HeatAbsorberR3Toyama_h
#define xraySystem_HeatAbsorberR3Toyama_h

namespace constructSystem{
  class VacuumPipe;
}
class Simulation;

namespace xraySystem
{

/*!
  \class HeatAbsorberR3Toyama
  \version 1.0
  \author Udo Friman-Gayer
  \date December 2025
  \brief Heat absorber in Toyama front-ends at R3 ring
*/

class HeatAbsorberR3Toyama:
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  std::shared_ptr<constructSystem::VacuumPipe> frontPipe;
  std::shared_ptr<constructSystem::VacuumPipe> backPipe;

  // Outer dimensions
  double length;
  double flangeRadius;

  // Absorber dimensions
  double absorberLength;
  double absorberWidth;
  double absorberHeight;
  double absorberConnectorLength;

  // Gap dimensions
  double gapWidth;
  double gapMinHeight;
  double gapMaxHeight;

  bool closed;                  ///< open/closed flag

  int mainMat;                  ///< Main material
  int voidMat;                  ///< Void material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  HeatAbsorberR3Toyama(const std::string&);
  HeatAbsorberR3Toyama(const HeatAbsorberR3Toyama&);
  HeatAbsorberR3Toyama& operator=(const HeatAbsorberR3Toyama&);
  virtual HeatAbsorberR3Toyama* clone() const;
  virtual ~HeatAbsorberR3Toyama();
  double getLength(){return length;}

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
