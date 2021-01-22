/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   LinacInc/StriplineBPM.h
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
#ifndef tdcSystem_StriplineBPM_h
#define tdcSystem_StriplineBPM_h

class Simulation;


namespace tdcSystem
{
/*!
  \class StriplineBPM
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief StriplineBPM for Max-IV
*/

class StriplineBPM :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]

  double outerThick;            ///< pipe thickness

  double innerRadius;           ///< inner radius
  double innerThick;            ///< Inner electorn thickness
  double innerAngle;            ///< Angle of stripline
  double innerAngleOffset;      ///< Offset angle of inner electron

  double flangeARadius;         ///< Joining Flange radius
  double flangeALength;         ///< Joining Flange length

  double flangeBRadius;         ///< Joining Flange radius
  double flangeBLength;         ///< Joining Flange length

  double striplineRadius;       ///< Stripline distance [support]
  double striplineThick;        ///< Stripline thickness [support]
  double striplineYStep;        ///< Stripline YStep
  double striplineEnd;          ///< Stripline end piece length

  int voidMat;                  ///< void material
  int striplineMat;             ///< stripline material
  int flangeMat;                ///< flange material
  int outerMat;                 ///< pipe material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  StriplineBPM(const std::string&);
  StriplineBPM(const std::string&,const std::string&);
  StriplineBPM(const StriplineBPM&);
  StriplineBPM& operator=(const StriplineBPM&);
  virtual ~StriplineBPM();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
