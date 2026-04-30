/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/danmaxInc/SqrShield.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell and Udo Friman-Gayer
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
 *5
 ****************************************************************************/
#ifndef xraySystem_SqrShield_h
#define xraySystem_SqrShield_h

class Simulation;

namespace xraySystem
{

/*!
  \class SqrShield
  \version 2.0
  \author Stuart Ansell and Udo Friman-Gayer
  \date 2019-2026
  \brief Shielding around beam axis with rectangular cross section

  The shielding is assumed to be centered around the beam axis, but the shielding
  thickness for each of the four sides can be set individually.
  At a user-defined position along the beam axis, the SqrShield can have a region 
  with a larger width and height.

  The SqrShield is used in the DanMAX beamline to shield the transfer line from the
  Optics Hutch to the Experimental Hutch 1. In DanMAX, the enlarged region contains
  a pump.

  Version History:
  2.0 - 2026-01-14
    - More customization:
      - Individual wall-thickness setting for each side.
      - Region with extended cross section 
        (but build zone determined by region with small cross section).
  1.0 - 2019  
*/

class SqrShield :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double smallWidth, largeWidth;    ///< Width of inner void
  double smallHeight, largeHeight;  ///< Height of inner void
  double length;                    ///< Total length [unused if F/B set]
  double largeRegionStart;          ///< Start of large region (distance from front)
  double largeRegionLength;         ///< Length of inner void of large section
   ///< Wall thickness
  double topThick, bottomThick, leftThick, rightThick, frontBackThick;
  double skinThick;                 ///< Skin thickness around wall

  int mat;                ///< Main wall materails
  int skinMat;            ///< Skin material
  int voidMat;            ///< Void material
  
  double dXYZ(const unsigned int, const unsigned int) const;

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SqrShield(const std::string&);
  SqrShield(const SqrShield&);
  SqrShield& operator=(const SqrShield&);
  ~SqrShield() override;

  HeadRule getSurround() const;
  int getInnerMat() const { return voidMat; }
  
  /// set delay
  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;

};

}

#endif


