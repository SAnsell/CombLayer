/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/TS2target.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef TMRSystem_TS2target_h
#define TMRSystem_TS2target_h

class Simulation;

namespace ts1System
{
  class ProtonVoid;
  class BeamWindow;
}

namespace TMRSystem
{
/*!
  \class TS2target
  \version 1.0
  \author S. Ansell
  \date December 2010
  \brief Creates the TS2 target [with nose cone]

  Provides linkage to its outside on FixedComp[0]
*/

class TS2target : public constructSystem::TargetBase
{
 private:
  
  const int protonIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index

  int frontPlate;               ///< Front Plate
  int backPlate;                ///< Back Plate
  
  double mainLength;            ///< Straight length
  double coreRadius;            ///< Inner W radius [cyl]
  double surfThick;             ///< Skin thickness on the W main core
  double wSphDisplace;          ///< Displacement of the sphere [Y]
  double wSphRadius;            ///< Radius of the W-sphere 
  double wPlaneCut;             ///< Plane cut distance
  double wPlaneAngle;           ///< Plane rotation angle

  double cladThick;             ///<  Thickness of cladding
  double taSphDisplace;         ///< Displacement of the sphere [Y]
  double taSphRadius;           ///< Radius of the W-sphere 
  double taPlaneCut;            ///< Plane cut distance-

  double waterThick;            ///< Thickness of the water
  double pressureThick;         ///< Thickness of the Ta pressure outer

  double xFlowCyl;              ///< Cross Flow outer base cyliner
  double xFlowOutRadius;        ///< Full outer radius
  double xFlowOutDisplace;      ///< Full outer displacement Y  

  double xFlowOutMidRadius;     ///< Cutter of sphere
  double xFlowOutCutAngle;      ///< Rotation angle
  double xFlowOPlaneCut;        ///< Cut of plane

  double xFlowInnerRadius;      ///< Cutter of inner water sphere
  double xFlowInnerDisplace;    ///< Displacement of sphere
  double xFlowIPlaneCut;        ///< Cut of plane
  
  double tCapInnerRadius;       ///< Inner Radius
  double tCapOuterRadius;       ///< Outer Radius
  double tCapDisplace;          ///< Outer Displacement [both]

  double voidRadius;            ///< Void radius [full outer]

  double flangeThick;           ///< Flange metal Thickness
  double flangeLen;             ///< Flange void length
  double flangeRadius;          ///< Flange Radius
  double flangeClear;           ///< Flange Radius
  double flangeYStep;           ///< Flange Y-Step
  int flangeMat;                ///< Flange material [steel?]
  
  int wMat;                     ///< Tungsten material
  int taMat;                    ///< Tatalium material
  int waterMat;                 ///< Target coolant material

  double targetTemp;            ///< Target temperature
  double waterTemp;             ///< Water temperature
  double externTemp;            ///< Pressure temperature

  int mainCell;                 ///< Main tungsten cylinder
  int skinCell;                 ///< Main ta cylinder

  size_t nLayers;               ///< number of layers
  std::vector<double> mainFrac; ///< Main fraction

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();  
  void createNoseConeSurfaces();
  void createObjects(Simulation&);
  void createNoseConeObjects(Simulation&);
  void createLinks();


 public:

  TS2target(const std::string&);
  TS2target(const TS2target&);
  TS2target& operator=(const TS2target&);
  virtual TS2target* clone() const; 
  virtual ~TS2target();

  /// Main cell body
  virtual int getMainBody() const  { return mainCell; }
  /// Ta cell body
  virtual int getSkinBody() const { return skinCell; }

  void addInnerBoundary(attachSystem::ContainedComp&) const;
  /// Set the extext of the reflector
  void setRefPlates(const int A,const int B) 
    { frontPlate=A; backPlate=B; }

  void addProtonLine(Simulation&,	 
		     const attachSystem::FixedComp& refFC,
		     const long int index);
  void layerProcess(Simulation&);
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&);
  

};

}

#endif
 
