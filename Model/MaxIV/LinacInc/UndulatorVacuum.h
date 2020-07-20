/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/UndulatorVacuum.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef tdcSystem_UndulatorVacuum_h
#define tdcSystem_UndulatorVacuum_h

class Simulation;

namespace xraySystem
{
  class FMUndulator;
}

namespace tdcSystem
{
/*!
  \class UndulatorVacuum.h
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief UndulatorVacuum.h for Max-IV
*/

class UndulatorVacuum :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  size_t nSegment;              ///< number of segments

  double radius;                ///< void radius
  double segLength;             ///< void length [total]
  double wallThick;             ///< Main wall thickness
  
  double flangeRadius;          ///< flange radius
  double flangeLength;          ///< Flange length (half)

  double portOutLength;         ///< horizontal port step (front/back)
  double portRadius;            ///< Exit port radius
  
  // 3 access ports a segment
  double accessRadius;          ///< Large Access port radius
  double accessFlangeRadius;    ///< Large Access port F. radius
  double accessFlangeLength;    ///< Large Access port F. Length

  double smallRadius;           ///< Small port radius
  double smallFlangeRadius;     ///< Small port F. radius
  double smallFlangeLength;     ///< Small port F. length

  // 6 magnet ports a segment[vertical]
  double magGap;                ///< Magnet Support gap
  double magRadius;             ///< Magnet Support port radius
  double magLength;             ///< Magnet Support port length
  double magBellowLen;          ///< Mag bellow length (full - real.bellow/2)
  double magFlangeRadius;       ///< Magnet Support port flange 
  double magFlangeLength;       ///< Magnet Support port flange length

  double magSupportRadius;      ///< Magnet Support radius
  
  // one at 2/5 distacne
  double vacRadius;             ///< Vacuum/Ion port radius
  double vacLength;             ///< Vacuum/Ion port length
  double vacHeight;             ///< Vacuum/Ion port upturn height
  double vacFlangeRadius;       ///< Vacuum/Ion port F.radius
  double vacFlangeLength;       ///< Vacuum/Ion port F.length
  
  double preLength;             ///< pre-pipe length

  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  int supportMat;               ///< support material

  std::shared_ptr<xraySystem::FMUndulator> undulator;
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createSupport(Simulation&);
  void createLinks();

 public:

  UndulatorVacuum(const std::string&);
  UndulatorVacuum(const std::string&,const std::string&);
  UndulatorVacuum(const UndulatorVacuum&);
  UndulatorVacuum& operator=(const UndulatorVacuum&);
  virtual ~UndulatorVacuum();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
