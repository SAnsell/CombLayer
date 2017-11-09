/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/SideCoolTarget.h
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
#ifndef ts1System_SideCoolTarget_h
#define ts1System_SideCoolTarget_h

class Simulation;

namespace ts1System
{
  class ProtonVoid;
  class BeamWindow;
}

namespace ts1System
{
/*!
  \class SideCoolTarget
  \version 1.0
  \author S. Ansell
  \date December 2010
  \brief Creates the TS2 target [with nose cone]

  Provides linkage to its outside on FixedComp[0]
*/

class SideCoolTarget : public constructSystem::TargetBase
{
 private:
  
  const int tarIndex;           ///< Index of surface offset

  int cellIndex;                ///< Cell index
  
  double mainLength;            ///< Straight length
  
  double xRadius;               ///< Inner W radius [elipse]
  double zRadius;               ///< Inner W radius [elipse]
  double cladThick;             ///< Skin thickness on the W main core
  double waterThick;            ///< Water thinkess
  double pressThick;            ///< Pressure thinkess
  double voidThick;             ///< Void around target
  double cutAngle;              ///< Angle of cut

  int wMat;                     ///< Tungsten material
  int taMat;                    ///< Tatalium material
  int waterMat;                 ///< Target coolant material

  double targetTemp;            ///< Target temperature
  double waterTemp;             ///< Water temperature
  double externTemp;            ///< Pressure temperature

  int mainCell;                 ///< Main tungsten cylinder

  size_t nLayers;               ///< number of layers
  std::vector<double> mainFrac; ///< Main fraction

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  SideCoolTarget(const std::string&);
  SideCoolTarget(const SideCoolTarget&);
  SideCoolTarget& operator=(const SideCoolTarget&);
  virtual SideCoolTarget* clone() const; 
  virtual ~SideCoolTarget();

  /// Main cell body
  int getMainBody() const { return tarIndex+1; }
  virtual std::vector<int> getInnerCells() const;
  void addInnerBoundary(attachSystem::ContainedComp&) const;

  void addProtonLine(Simulation&,	 
		     const attachSystem::FixedComp& refFC,
		     const long int index);
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&);
  

};

}

#endif
 
