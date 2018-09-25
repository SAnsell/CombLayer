/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/PBW.h
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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
#ifndef essSystem_PBW_h
#define essSystem_PBW_h

class Simulation;

namespace essSystem
{
  class TelescopicPipe;

/*!
  \class PBW
  \version 1.1
  \author Konstantin Batkov
  \date Apr 2018
  \brief Proton beam window
*/


class PBW : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  int engActive;                ///< Engineering active flag

  double plugLength1;                ///< phalf-length of PBW lug Length (+- ymax)
  double plugLength2; ///< half-length of PBW plug in the ymin direction
  double plugWidth1;                 ///< Width1 (+- x at ymax)
  double plugWidth2; ///< plug width2 (+-x at ymin)
  double plugHeight;                ///< height
  double plugDepth; ///< plug depth (-z)
  int plugMat; ///< plug material
  double plugVoidLength; ///< plug void length
  double plugVoidWidth; ///< plug void width
  double plugVoidDepth; ///< plug void depth
  double plugVoidHeight; ///< plug void height
  double plugAlLength; ///< length of Al plate in the plug
  double plugAlGrooveRadius; ///< groove radius in the Al plate
  double plugAlGrooveDepth; ///< groove depth in the Al plate
  double plugAlGapHeight; ///< rectangular gap full height in the Al plate
  double plugAlGapWidth; ///< rectangular gap full width in the Al plate

  double flangeRadius; ///< inner flange radius
  double flangeThick; ///< flange thickness
  double flangeWaterRingRadiusIn; ///< inner radius of water cooling if PBW flange
  double flangeWaterRingRadiusOut; ///< outer radius of water cooling if PBW flange
  double flangeWaterRingThick; ///< thickness of each of the PBW flange water rings
  double flangeWaterRingOffset; ///< offset of PBW flange water rings from the side planes
  double flangeNotchDepth; ///< depth of cuts in the flange cylinder
  double flangeNotchThick; ///< length of cut in the flange cylinder
  double flangeNotchOffset; ///< offset of cuts in the flange cylinder from the side planes

  double foilThick; ///< thickness of PBW foil
  double foilRadius; ///< smaller radius of PBW foil
  double foilOffset; ///< distance from the Al side plane and nearest foil plane along the y-axis
  double foilCylOffset; ///< y-axis offset of foil cylinder centre
  double foilWaterThick; ///< thickness of water layer inside foil
  double foilWaterLength; ///< length (along z-axis chorda) of water layer inside foil

  double pipeRad;             ///< inner radius of proton tube containing the PBW
  int pipeMatBefore; ///< material inside the proton tube before PBW
  int pipeMatAfter; ///< proton tube material after PBW

  int coolingMat;                   ///< cooling material
  int mat;                   ///< PBW material
  std::shared_ptr<TelescopicPipe> shield; ///< Shielding

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int&);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);


 public:

  PBW(const std::string&);
  PBW(const PBW&);
  PBW& operator=(const PBW&);
  virtual PBW* clone() const;
  virtual ~PBW();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int&,
		 const attachSystem::FixedComp&,const long int&);

};

}

#endif


