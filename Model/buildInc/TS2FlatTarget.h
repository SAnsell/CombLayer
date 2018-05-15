/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/TS2FlatTarget.h
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
#ifndef TMRSystem_TS2FlatTarget_h
#define TMRSystem_TS2FlatTarget_h

class Simulation;

namespace ts1System
{
  class ProtonVoid;
  class BeamWindow;
}

namespace TMRSystem
{
/*!
  \class TS2FlatTarget
  \version 1.0
  \author S. Ansell
  \date June 2014
  \brief Creates the TS2 target [without nose cone]

  Provides linkage to its outside on FixedComp[0]
*/

class TS2FlatTarget : public constructSystem::TargetBase
{
 private:
  
  const int protonIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index

  int frontPlate;               ///< Front Reflector surf
  int backPlate;                ///< Back Reflector surf
  
  double mainLength;            ///< Straight length
  double coreRadius;            ///< Inner W radius [cyl]
  double surfThick;             ///< Skin thickness on the W main core

  double cladThick;             ///<  Thickness of cladding
  double cladFront;             ///<  Thickness of front

  double waterThick;            ///< Thickness of the water
  double waterFront;            ///< Thickness of the water [front]
  double pressureThick;         ///< Thickness of the Ta pressure outer
  double pressureFront;         ///< Thickness of the Ta pressure outer

  double voidRadius;            ///< Void radius [full outer]
  double voidFront;             ///< Void front thick

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
  void createObjects(Simulation&);
  void createLinks();

 public:

  TS2FlatTarget(const std::string&);
  TS2FlatTarget(const TS2FlatTarget&);
  TS2FlatTarget& operator=(const TS2FlatTarget&);
  virtual TS2FlatTarget* clone() const; 
  virtual ~TS2FlatTarget();

  /// Main cell body
  int getMainBody() const  { return mainCell; }
  /// Ta cell body
  int getSkinBody() const { return skinCell; }

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
 
