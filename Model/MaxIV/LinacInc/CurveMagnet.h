/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/CurveMagnet.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef tdcSystem_CurveMagnet_h
#define tdcSystem_CurveMagnet_h

class Simulation;

namespace tdcSystem
{

/*!
  \class CurveMagnet
  \version 1.0
  \author Konstantin Batkov
  \date Apr 2020
  \brief DIB Dipole magnet
*/

class CurveMagnet :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedGroup,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:

  double poleGap;               ///< Gap on pole
  double poleHeight;            ///< Gap height on pole
  double poleAngle;             ///< Angle on pole pieces cone

  double coilGap;               ///< gap between coils
  double coilArcLength;         ///< Arc lengh [straight-line]
  double coilArcRadius;         ///< radius of main curve
  double coilDepth;             ///< depth across coil
  double coilWidth;             ///< Width of coil

  int poleMat;                  ///< Pole material
  int coilMat;                  ///< Coil material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  CurveMagnet(const std::string&);
  CurveMagnet(const CurveMagnet&);
  CurveMagnet& operator=(const CurveMagnet&);
  virtual ~CurveMagnet();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
