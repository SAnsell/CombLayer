/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   danmaxInc/MLMono.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_MLMono_h
#define xraySystem_MLMono_h

class Simulation;

namespace xraySystem
{

/*!
  \class MLMono
  \author S. Ansell
  \version 1.0
  \date October 2019
  \brief Double Mirror Mono arrangement
*/

class MLMono :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:

  double gap;               ///< Gap thickness

  double thetaA;             ///< Theta angle [mirror -xray 2T]
  double thetaB;             ///< Theta angle [mirror -xray 2T]
  double phiA;             ///< Theta angle [mirror -xray 2T]
  double phiB;             ///< Theta angle [mirror -xray 2T]

  double widthA;            ///< Width of block across beam
  double heightA;           ///< Depth into beam
  double lengthA;           ///< Length along beam
  
  double widthB;            ///< Width of block across beam
  double heightB;           ///< Depth into beam
  double lengthB;           ///< Length along beam

  double supportAGap;         ///< Gap after mirror (before back)
  double supportAExtra;       ///< Base/Top extra length
  double supportABackThick;   ///< Back thickness
  double supportABackLength;  ///< Back lenght (in part)
  double supportABase;        ///< Base/Top thickness
  double supportAPillar;      ///< Side unit radius
  double supportAPillarStep;  ///< Side unit step

  double supportBGap;         ///< Gap after mirror (before back)
  double supportBExtra;       ///< Base/Top extra length
  double supportBBackThick;   ///< Back thickness
  double supportBBackLength;  ///< Back lenght (in part)
  double supportBBase;        ///< Base/Top thickness
  double supportBPillar;      ///< Side unit radius
  double supportBPillarStep;  ///< Side unit step

  int mirrorAMat;             ///< XStal material
  int mirrorBMat;             ///< XStal material
  int baseAMat;               ///< Base material
  int baseBMat;              ///< Base material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  MLMono(const std::string&);
  MLMono(const MLMono&);
  MLMono& operator=(const MLMono&);
  virtual ~MLMono();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
