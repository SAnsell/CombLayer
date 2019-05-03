/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/DipoleChamber.h
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
#ifndef xraySystem_DipoleChamber_h
#define xraySystem_DipoleChamber_h

class Simulation;


namespace xraySystem
{
/*!
  \class DipoleChamber
  \version 1.0
  \author S. Ansell
  \date January 2019

  \brief DipoleChamber for Max-IV 

  This is built relative to the proton channel
*/

class DipoleChamber :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  double length;                ///< frame length
  double outWidth;               ///< Step 
  double ringWidth;             ///< Step ring side at start
  double curveRadius;           ///< Radius of ring side curve
  double curveAngle;           ///< Total angle [deb]
  double height;                ///< Inner height [straight]

  double exitWidth;              ///< Cut in exit channel
  double exitHeight;             ///< Cut in exit channel
  double exitLength;             ///< Length of exit channel

  double flangeRadius;          ///< Joining Flange radius
  double flangeLength;          ///< Joining Flange length

  double wallThick;             ///< Wall thickness


  double innerXFlat;            ///< First X flat inside
  double innerXOut;             ///< First X out side

  double elecXFlat;             ///< Flat cut
  double elecXCut;              ///< Curve on phon side
  double elecXFull;             ///< Curve on electron side
  
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  int innerMat;                 ///< inner material
  int flangeMat;                ///< flange material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DipoleChamber(const std::string&);
  DipoleChamber(const DipoleChamber&);
  DipoleChamber& operator=(const DipoleChamber&);
  virtual ~DipoleChamber();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
