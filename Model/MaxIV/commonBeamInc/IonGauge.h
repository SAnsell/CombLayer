/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/IonGauge.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef tdcSystem_IonGauge_h
#define tdcSystem_IonGauge_h

class Simulation;


namespace xraySystem
{

  /*!
  \class IonGauge
  \version 1.0
  \author S. Ansell
  \date May 2020

  \brief IonGauge for Max-IV
*/

class IonGauge :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double radius;                ///< void radius [vertical]
  double xRadius;               ///< radius of X (Side) port
  double yRadius;               ///< radius of Y (Beam) port

  double wallThick;             ///< pipe thickness

  double height;                ///< full to blank
  double depth;                 ///< full to flange depth below beam

  double frontLength;           ///< full to flange length (-ve Y)
  double backLength;            ///< full to flange length

  double flangeXRadius;         ///< Side join flange
  double flangeYRadius;         ///< Beam flange radius
  double flangeZRadius;         ///< Main radius

  double flangeXLength;         ///< Side Flange length
  double flangeYLength;         ///< Beam Flange length
  double flangeZLength;         ///< Main Flange length

  double sideZOffset;           ///< Second Z lift
  double sideLength;            ///< Second Z length

  double gaugeZOffset;          ///< Gauge Z offset
  double gaugeRadius;           ///< Gauge Z offset
  double gaugeLength;           ///< Gauge Length
  double gaugeHeight;           ///< Gauge Height up
  double gaugeFlangeRadius;     ///< Gauge flange thickness
  double gaugeFlangeLength;     ///< Gauge flange thickness

  double plateThick;            ///< Top/Base plate thick

  int voidMat;                  ///< void material
  int wallMat;                  ///< main material
  int plateMat;                 ///< plate material
 
  void populate(const FuncDataBase&) override;  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  IonGauge(const std::string&);
  IonGauge(const std::string&,const std::string&);
  IonGauge(const IonGauge&);
  IonGauge& operator=(const IonGauge&);
  ~IonGauge() override;

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;

};

}

#endif
 
