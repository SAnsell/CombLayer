/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WWGConstructor.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef WeightSystem_WWGConstructor_h
#define WeightSystem_WWGConstructor_h

namespace attachSystem
{
  class FixedComp;
}

class Simulation;

namespace WeightSystem
{

      
/*!
  \class WWGConstruct
  \version 1.0
  \author S. Ansell
  \date October 2015
  \brief WWG constructor

  Construct a WWG system wwinp output
*/

class WWGconstruct
{
  public:

  WWGconstruct();
  WWGconstruct(const WWGconstruct&);
  WWGconstruct& operator=(const WWGconstruct&);
  virtual ~WWGconstruct() {}  ///< Destructor


  void createWWG(Simulation&,const mainSystem::inputParam&);
};

}

#endif
  
