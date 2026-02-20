/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoShutterR3.h
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#ifndef xraySystem_MonoShutterR3_h
#define xraySystem_MonoShutterR3_h

class Simulation;

namespace xraySystem
{
/*!
  \class MonoShutterR3
  \author U. Friman-Gayer
  \version 1.1
  \date February 2026
  \brief Monochromatic shutter system for hard x-ray beamlines at the R3 ring of MAX IV

  The shutter system consists of the following elements:
  * PipeTube with the following four ports
    * Port0: Entry ("front") including a flange adapter
    * Port1: Exit ("back") including a flange adapter
    * Port2: Shutter 1 on entry side
    * Port3: Shutter 2 on exit side
  * Two shutter units at the respective ports
  * Three disk-shaped fixed apertures at the following positions
    * Upstream from shutter 1
    * Between shutter 1 and 2
    * Downstream from shutter 2. This last aperture has an additional part that 
      extends into the exit flange.

  References:
  [1] Technical Specifications of mono beam shutter for MAX-IV, AXILON, Rp-3669-1173-0, 2025-07-25
  [2] Drawing of shutter block, 15-01-999952, AXILON, 2025-08-21
  [3] Drawing of standard aperture, 25-01-1120795, AXILON, 2025-08-21
  [4] Drawing of downstream aperture, 25-01-1120798, AXILON, 2025-08-21
  [5] CAD model of ForMAX/MicroMAX/DanMAX mono shutters

  Version history:
  1.1 - 2026-11-02
    - Derive from ContainedComp instead of ContainedGroup
    - Simplified outer volume
  1.0 - 2026-10-02
*/

class MonoShutterR3 :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::SurfMap,
  public attachSystem::CellMap
{
 private:

  double height;
  double length;
  double width;

  double adapterInnerRadius;

  double beamPortInnerRadius;
  double beamPortWallThick;
  double beamPortFlangeRadius;
  double beamPortFlangeLength;

  double vesselInnerRadius;
  double vesselWallThick;
  double vesselFlangeRadius;
  double vesselFlangeLength;
  int vesselMat;

  double apertureBackLength;
  double apertureInnerRadius;
  double apertureOuterRadius;
  double apertureThick;
  int apertureMat;
  double apertureToBlockGap;

  double blockHeight;
  double blockLength;
  double blockWidth;
  int blockMat;

  double shutterDistance;
  double shutterPortLength;
  double shutterPortInnerRadius;
  double shutterPortWallThick;
  double shutterPortFlangeRadius;
  double shutterPortFlangeLength;
  double threadLength;
  double threadRadius;
  int threadMat;
  double lift;

  bool entryShutterUpFlag;
  bool exitShutterUpFlag;

 protected:
  void populate(const FuncDataBase&) override;

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  MonoShutterR3(const std::string&);

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};

}

#endif
 
