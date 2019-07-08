/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/RingDoor.h
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
#ifndef xraySystem_RingDoor_h
#define xraySystem_RingDoor_h

class Simulation;

namespace xraySystem
{
  /*!
    \class RingDoor
    \version 1.0
    \author S. Ansell
    \date June 2018
    \brief Door for the outer wall of the R1/R3 rings
  */
  
class RingDoor :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:

  double innerHeight;                 ///< height of inner gap
  double innerWidth;                  ///< width of inner gap
  double innerThick;                  ///< thickness of first door

  double innerTopGap;                 ///< top gap inner
  double outerTopGap;                 ///< top gap outer
  double gapSpace;                    ///< gap
 
  double outerHeight;                 ///< height of outer gap
  double outerWidth;                  ///< width of outer gap

  double tubeRadius;                  ///< Radius of top tubes
  double tubeXStep;                   ///< X of top tubes
  double tubeZStep;                   ///< Z of top tubes

  double underStepHeight;             ///< height of gap
  double underStepWidth;              ///< Width of under gap
  double underStepXSep;               ///< X separation of steps

  int tubeMat;                        ///< tube material material
  int doorMat;                        ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  RingDoor(const std::string&);
  RingDoor(const RingDoor&);
  RingDoor& operator=(const RingDoor&);
  virtual ~RingDoor() {}  ///< Destructor


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
