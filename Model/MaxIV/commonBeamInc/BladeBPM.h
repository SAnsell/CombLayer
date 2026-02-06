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
    public attachSystem::ContainedGroup,
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
  double insertFlangeRadius;    ///< Blades insert flange radius
  double insertFlangeLength;    ///< Blades insert flange length
  double insertInnerRadius;     ///< Blades insert inner radius
  double insertOuterRadius;     ///< Blades insert outer radius
  double insertPreOuterRadius;  ///< Outer radius of the insert part containing blades (in the vacuum chamber)
  double insertLength;          ///< Blades insert length

  double portLength;            ///< Feedthrough port length (distance between beam axis and flange upper plane, without its cap)
  double portWallThick;         ///< Feedthrough port wall thickness
  double portRadius;            ///< Feedthrough port inner radius
  double portCapLength;         ///< Feedthrough port cap length
  double portCapCentralLength;  ///< Feedthrough port cap central part length
  double portCapCentralRadius;  ///< Feedthrough port cap central part radius
  double portFlangeLength;      ///< Feedthrough port flange length
  double portFlangeRadius;      ///< Feedthrough port flange radius

  int chamberFlangeMat;         ///< Vacuum chamber flange material
  int chamberWallMat;           ///< Vacuum chamber wall material
  int insertMat;                ///< Blades insert material
  int insertPreMat;             ///< Homogenised material of the insert part with blades
  int insertFlangeMat;          ///< Blades insert flange material
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
