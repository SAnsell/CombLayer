/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/RingDoor.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
    \version 1.1
    \author S. Ansell and U. Friman-Gayer
    \date June 2026
    \brief Door for the outer wall of the R1/R3 rings

  Version History:
  1.1 - 2026-06-04
    - More flexible control of gap sizes.
  1.0 - 2018-06
  */
  
class RingDoor :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap

{
 private:

  double innerHeight;                 ///< height of inner gap
  double innerWidth;                  ///< width of inner gap
  double innerThick;                  ///< thickness of first door

  double innerTopGap;                 ///< top gap inner
  double outerTopGap;                 ///< top gap outer
  double gapSpace;                    ///< gap
  // By design, the door should meet the wall where the step from inner to outer part
  // is, and then be level with the inner and outer side of the wall. In reality, there
  // will be a small gap left whose size is controlled by this parameter.
  // Instead of letting the door protude towards the outside for a nonzero value of
  // stepGapSpace, this model instead reduces (increases) the size of the outer (inner)
  // part by stepGapSpace so that the door is still level with the walls.
  double stepGapSpace;            ///< gap at step from inner to outer part (ideally 0)
 
  double outerHeight;                 ///< height of outer gap
  double outerWidth;                  ///< width of outer gap

  double tubeRadius;                  ///< Radius of top tubes
  double tubeXStep;                   ///< X of top tubes
  double tubeZStep;                   ///< Z of top tubes

  double underStepHeight;             ///< height of gap
  double underStepWidth;              ///< Width of under gap
  double underStepXSep;               ///< X separation of steps

  int underAMat;                      ///< Filling of under material
  int underBMat;                      ///< Filling of under material
  int tubeMat;                        ///< tube material material
  int doorMat;                        ///< wall material

  bool useTubes;                      ///< Controls construction of tubes

  void populate(const FuncDataBase&) override;
  void createUnitVector(const attachSystem::FixedComp&,
			const long int) override;
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  RingDoor(const std::string&);
  RingDoor(const RingDoor&);
  RingDoor& operator=(const RingDoor&);
  ~RingDoor() override {}  ///< Destructor

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
};

}

#endif
 
