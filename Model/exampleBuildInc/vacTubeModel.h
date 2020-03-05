/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exmapleInc/vacTubeModel.h
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
#ifndef exampleSystem_vacTubeModel_h
#define exampleSystem_vacTubeModel_h


/*!
  \namespace pipeSystem
  \brief simple pipemodl
  \version 1.0
  \date November 2019
  \author S. Ansell
*/
namespace constructSystem
{
  class VacuumPipe;
  class GateValveCylinder;
}

namespace exampleSystem
{
  class ShieldRoom;
  /*!
    \class vacTubeModle
    \version 1.0
    \author S. Ansell
    \date March 2020
    \brief General pipe for 10Hz model
  */

  
class vacTubeModel
{
 private:

  std::shared_ptr<exampleSystem::ShieldRoom> shieldRoom;   ///< shield room

  std::shared_ptr<constructSystem::VacuumPipe> pipeA;   ///< vac-tube
  std::shared_ptr<constructSystem::GateValveCylinder> gateA;   ///< gate valve
  std::shared_ptr<constructSystem::VacuumPipe> pipeB;   ///< vac-tube
    
 public:
  
  vacTubeModel();
  vacTubeModel(const vacTubeModel&);
  vacTubeModel& operator=(const vacTubeModel&);
  ~vacTubeModel();
  
  void build(Simulation&);

};

}

#endif
