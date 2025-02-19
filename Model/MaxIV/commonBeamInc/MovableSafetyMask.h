 /*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/MovableSafetyMask.h
 *
 * Copyright (c) 2004-2025 by Stuart Ansell / Konstantin Batkov
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
#ifndef xraySystem_MovableSafetyMask_h
#define xraySystem_MovableSafetyMask_h

class Simulation;

namespace xraySystem
{
  /*!
    \class MovableSafetyMask
    \version 1.0
    \author K. Batkov
    \date February 2025
    \brief Movable safety mask for use with two insertion devices (e.g. in the MAX IV TomoWISE beamline)

    Note that this object is CENTRE orientated.
    It is designed to exact placment.
  */

class MovableSafetyMask :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:

  double width;                 ///< Main radius
  double height;                ///< Main height
  double length;                ///< thickness of collimator

  double innerAWidth;           ///< front width
  double innerAHeight;          ///< front height

  double minLength;               ///< point of min closure
  double innerMinWidth;           ///< min width at closure
  double innerMinHeight;          ///< min height at closure

  double innerBWidth;            ///< back width
  double innerBHeight;           ///< back height

  double flangeAInRadius;        ///< Joining Flange inner radius
  double flangeAOutRadius;       ///< Joining Flange outer radius
  double flangeALength;          ///< Joining Flange length

  double flangeBInRadius;        ///< Joining Flange inner radius
  double flangeBOutRadius;       ///< Joining Flange outer radius
  double flangeBLength;          ///< Joining Flange length

  double pipeYStep[4];           ///< Step along model
  double pipeRadius;             ///< Cooling radius
  double pipeXWidth;             ///< Cooling length
  double pipeZDepth;             ///< Distance below beam

  int mat;                      ///< material
  int flangeMat;                ///< material of flange
  int waterMat;                 ///< water material

  int voidMat;                  ///< inner material

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MovableSafetyMask(const std::string&);
  MovableSafetyMask(const MovableSafetyMask&);
  MovableSafetyMask& operator=(const MovableSafetyMask&);
  ~MovableSafetyMask() override {}  ///< Destructor

  void populate(const FuncDataBase&) override;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
