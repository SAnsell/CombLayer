/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   buildInc/targetOuter.h
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
#ifndef snsSystem_targetOuter_h
#define snsSystem_targetOuter_h

class Simulation;

namespace snsSystem
{
/*!
  \class targetOuter
  \version 1.0
  \author S. Ansell
  \date December 2013
  \brief Creates the SNS target front nose cone
*/

class targetOuter : public constructSystem::TargetBase
{
 private:
  
  const int protonIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index
  
  double totalLength;            ///< Straight length

  double mainLength;            ///< Straight length
  double mainJoin;              ///< End of cone
  double mainHeight;            ///< Main Height
  double mainWidth;             ///< Main Width
  double innerWall;             ///< Inner containment layer

  double heliumLength;          ///< Start of cone 
  double heliumJoin;            ///< second flat length
  double heliumStep;            ///< Forward step of object
  double heliumXStep;           ///< Side-step of object
  double heliumThick;           ///< Thickness of He

  double pressLength;           ///< Start of cone
  double pressJoin;             ///< Straight length
  double pressStep;             ///< Forward step  disance
  double pressXStep;            ///< side-step
  double pressThick;            ///< steel thickness

  double waterLength;           ///< Start of cone
  double waterJoin;             ///< Straight length
  double waterStep;             ///< Out step  disance
  double waterXStep;             ///< Out step  disance
  double waterThick;            ///< Flat thickness

  double outerLength;           ///< Start of cone
  double outerJoin;             ///< Straight length
  double outerLift;             ///< Straight length
  double outerStep;             ///< Out step  disance  
  double outerXStep;             ///< Out step  disance
  double outerThick;            ///< Flat thickness

  double hgCutAngle;            ///< Cut angle of Mercury
  double innerCutAngle;         ///< Cut angle of (He initially)
  double heCutAngle;            ///< Cut angle of (He initially)
  double pressCutAngle;         ///< Cut angle of 
  double waterCutAngle;         ///< Cut angle of water
  double outerCutAngle;         ///< Cut angle of (He initially)

  int mercuryMat;               ///< Mercury material
  int innerWallMat;             ///< inner material [steel]
  int heMat;                    ///< He material 
  int pressMat;                 ///< inner pressure material [stee]
  int waterMat;                 ///< Water cooling material [D2O]
  int outerMat;                 ///< outer layer material [steel]

  double mercuryTemp;           ///< Mercury temperature

  int mainCell;                 ///< Main tungsten cylinder

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  
  void createSurfaces();  
  void createObjects(Simulation&);
  void createLinks();

 public:

  targetOuter(const std::string&);
  targetOuter(const targetOuter&);
  targetOuter& operator=(const targetOuter&);
  virtual targetOuter* clone() const; 
  virtual ~targetOuter();

  /// Main mercury cell body
  int getMainBody() const  { return mainCell; }

  void addProtonLine(Simulation&,const attachSystem::FixedComp&,
		     const long int);

  virtual void createAll(Simulation&,const attachSystem::FixedComp&);
  

};

}

#endif
 
