/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   exmapleInc/dipoleModel.h
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
#ifndef exampleSystem_dipoleModel_h
#define exampleSystem_dipoleModel_h


/*!
  \namespace pipeSystem
  \brief simple pipemodl
  \version 1.0
  \date November 2019
  \author S. Ansell
*/

namespace exampleSystem
{
  /*!
    \class makePipe
    \version 1.0
    \author S. Ansell
    \date May 2015
    \brief General pipe building system
  */
  class dipolePipe;
  class quadPipe;
  
class dipoleModel
{
 private:


  std::shared_ptr<exampleSystem::dipolePipe> DPipe;   ///< pre-tube
  std::shared_ptr<exampleSystem::quadPipe> QuadA;   ///< pre-tube
  std::shared_ptr<exampleSystem::quadPipe> QuadB;   ///< pre-tube
  std::shared_ptr<exampleSystem::quadPipe> QuadC;   ///< pre-tube
  std::shared_ptr<exampleSystem::quadPipe> QuadD;   ///< pre-tube
  
 public:
  
  dipoleModel();
  dipoleModel(const dipoleModel&);
  dipoleModel& operator=(const dipoleModel&);
  ~dipoleModel();
  
  void build(Simulation&);

};

}

#endif
