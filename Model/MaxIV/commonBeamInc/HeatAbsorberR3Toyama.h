/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/HeatAbsorberR3Toyama.h
 *
 * Copyright (c) 2004-2025 by Udo Friman-Gayer
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

class Simulation;

namespace xraySystem
{

/*!
  \class HeatAbsorberR3Toyama
  \version 1.0
  \author Udo Friman-Gayer
  \date December 2025
  \brief Heat absorber in Toyama front-ends at R3 ring

  The geometry is a simplified version of the drawing in Ref. [1], containing 
  the following parts:
  
  * The main heat-absorber block with a rectangular cross section. Inside, it has a 
    rectangular gap with a slanted upper side, i.e. its cross section decreases from
    front to back.
  * Flanges at the front and back. According to Ref. [1], they have CF63 standard 
    dimensions.
  * Short pieces with circular cross section between the flanges and the block.
  
  The entire object can either be in a closed or open position, which is determined
  by its location on the Z axis.

  [1] Toyama Drawing S5-2-1AJ00545
*/

class HeatAbsorberR3Toyama:
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  // Outer dimensions
  double length;
  double flangeRadius;

  // Connector dimensions
  double connectorInnerRadius;

  // Absorber dimensions
  double absorberLength;
  double absorberWidth;
  double absorberHeight;
  double absorberConnectorLength;

  // Gap dimensions
  double gapWidth;
  double gapMinHeight; ///< Gap height at the back.
  double gapMaxHeight; ///< Gap height at the front.

  double inOutRange;  ///< Distance between the IN and OUT positions.
                      ///< The position where the beam centered on the gap at the back
                      ///< is +inOutRange/2
  bool closed;                  ///< open/closed flag, corresponding to positioning at
                                ///< +inOutRange/2 and -inOutRange/2

  int mainMat;        ///< Main material
  int voidMat;        ///< Void material
  int pipeMat;        ///< Pipe material (wall + flange)

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
