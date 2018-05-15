/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1UpgradeInc/EngReflector.h
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
#ifndef ts1System_EngReflector_h
#define ts1System_EngReflector_h

class Simulation;

namespace TMRSystem
{
  class TS2target;
  class TS2moly;
}

namespace ts1System
{

/*!
  \class EngReflector
  \version 1.0
  \author G. Skoro
  \date January 2015
  \brief TS1 Upgrade Reflector - Engeeniring version
*/

class EngReflector : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int refIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double radius;                ///< Reflector radius
  double width;                 ///< Reflector full width 
  double height;                ///< Reflector full height
  double topCutHeight;          ///< cut height (from top)
  double botCutHeight;          ///< cut height (from bottom) 
  double cutLen;                ///< Reflector (4) cuts length
  double cutAngle;              ///< Reflector (4) cuts angle  
  double topCCHeight;           ///< Reflector cylindrical cut - Height (from top)
  double cCoff;                 ///< Reflector cylindrical cut - Offset
  double cutRadius;             ///< Reflector cylindrical cut - Radius     
  int reflMat;                  ///< Reflector material
  double reflTemp;              ///< Reflector temperature

  double liftHeight;         ///< lifting plate height
  int liftMat;               ///< lifting plate material
  double liftTemp;           ///< lifting plate temperature 
  
  double coolPadThick;          ///< Cooling pad thickness
  double coolPadOffset;         ///< Cooling pad width offset
  double coolPadWidth;          ///< Cooling pad full width
  double coolPadHeight;         ///< Cooling pad height (from top)
  int coolPadMat;               ///< Cooling pad material
  double coolPadTemp;           ///< Cooling pad temperature 

  double inCutXOffset;         ///< cut X offset - Cold Moderators
  double inCutYOffset;         ///< cut Y offset  
  double inCutZOffset;         ///< cut Z offset
  double inCutThick;          ///< cut thickness
  double inCutWidth;         ///< cut width
  double inCutAngle;         ///< cut angle  
  double inCutHeight;         ///< cut height
  int inCutMat;         ///< cut material
  double inCutTemp;         ///< cut temperature     

  double inBWatCutXOffset;         ///< cut X offset - Water Moderators
  double inBWatCutYOffset;         ///< cut Y offset  
  double inBWatCutZOffset;         ///< cut Z offset
  double inBWatCutHeight;         ///< cut height
  double inBWatCutRadius;         ///< cut radius  
  int inBWatCutMat;         ///< cut material
  double inBWatCutTemp;         ///< cut temperature    

  double inSWatCutXOffset;         ///< cut X offset - Water Moderators
  double inSWatCutYOffset;         ///< cut Y offset  
  double inSWatCutRadius;         ///< cut radius  
        
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  EngReflector(const std::string&);
  EngReflector(const EngReflector&);
  EngReflector& operator=(const EngReflector&);
  virtual ~EngReflector();

  std::string getComposite(const std::string&) const;
  void addToInsertChain(attachSystem::ContainedComp& CC) const;
  // Main cell
  int getInnerCell() const { return refIndex+1; }
  std::vector<int> getCells() const;
  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
