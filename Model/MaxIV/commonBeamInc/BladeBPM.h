/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/BladeBPM.h
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#ifndef xraySystem_BladeBPM_h
#define xraySystem_BladeBPM_h

class Simulation;

namespace xraySystem
{

/*!
  \class BladeBPM
  \version 1.0
  \author Konstantin Batkov
  \date February 2026
  \brief Photoemission Blade Beam Position Monitor (XBPM) by Toyama
*/

class BladeBPM :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including XBPM insert and vacuum chamber
  double chamberLength;         ///< Vacuum chamber length including flanges
  double chamberRadius;         ///< Vacuum chamber radius (inner)
  double chamberWallThick;      ///< Vacuum chamber wall thickness
  double chamberFlangeRadius;   ///< Vacuum chamber flange radius
  double chamberFlangeLength;   ///< Vacuum chamber flange length
  double insertFlangeRadius;    ///< Insert flange radius
  double insertFlangeLength;    ///< Insert flange length
  double insertInnerRadius;     ///< Insert inner radius
  double insertOuterRadius;     ///< Insert outer radius
  double insertPreOuterRadius;  ///< Outer radius of the insert part containing blades (in the vacuum chamber)
  double insertLength;          ///< Insert length

  int chamberFlangeMat;         ///< Vacuum chamber flange material
  int chamberWallMat;           ///< Vacuum chamber wall material
  int insertMat;                ///< Insert material
  int insertPreMat;             ///< Homogenised material of the insert part with blades
  int insertFlangeMat;          ///< Insert flange material
  int voidMat;                  ///< Void material
  int airMat;                   ///< Air material

  void populate(const FuncDataBase&);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  BladeBPM(const std::string&);
  BladeBPM(const BladeBPM&);
  BladeBPM& operator=(const BladeBPM&);
  virtual BladeBPM* clone() const;
  virtual ~BladeBPM();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
