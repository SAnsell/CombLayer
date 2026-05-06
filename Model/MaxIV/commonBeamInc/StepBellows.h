/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeamInc/StepBellows.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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
#ifndef xraySystem_StepBellows_h
#define xraySystem_StepBellows_h

class Simulation;
class ContainedGroup;

namespace xraySystem
{

  class SmallAngleBellows;

/*!
  \class StepBellows
  \version 0.1
  \author U. Friman-Gayer
  \date May 2026
  \brief Cylindric bellows with parallel shift of incoming and outgoing axis

  The parallel shift ("step") of the main axis is achieved by combining two 
  SmallAngleBellows ("front" and "back") such that the angle of the second bellows 
  compensates the angle of the first.
  The length is the orthogonal distance between the front surface of the front bellows
  and the back surface of the back bellows. Please note that this is different from the
  definition of the "lengths" of the individual bellows. For a nonzero angle,
  StepBellows::length < FrontBellows::length + BackBellows::length.
  An illustrative example is shown below:

                       LENGTH
                 <---------------->
  incoming axis  ______
                       \               ^      Front Bellows (downward bend)
                        \              | S    Front Bellows (downward bend)
                         \             | T    Front Bellows (downward bend)
                          \            | E    Back  Bellows (upward bend)
                           \           | P    Back  Bellows (upward bend)
                            \______    v      Back  Bellows (upward bend) | outgoing axis

  For more information on coordinate systems and restrictions of the class, please
  refer to the documentation of the SmallAngleBellows class.
*/

class StepBellows:
  public attachSystem::FixedRotate,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{

 protected:
  double length;
  double step;

  std::shared_ptr<SmallAngleBellows> frontBellows;
  std::shared_ptr<SmallAngleBellows> backBellows;

  virtual void populate(const FuncDataBase&);
  void createSurfaces();
  void createLinks();

  void buildComponents(Simulation&);

 public:

  StepBellows(const std::string&);
  StepBellows(const StepBellows&);
  StepBellows& operator=(const StepBellows&);
  ~StepBellows();


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int) override;
};
}

#endif
