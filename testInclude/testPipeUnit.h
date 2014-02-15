/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testPipeUnit.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef testPipeUnit_h
#define testPipeUnit_h 

/*!
  \class testPipeUnit
  \brief Tests the pipeUnit system
  \author S. Ansell
  \date July 2011
  \version 1.0
  
  Test the component and building surfaces/object
*/

class testPipeUnit 
{
 private:

  Simulation ASim;           ///< Simulation model

  void initSim();
  void createSurfaces();
  void createObjects();
  //Tests 
  int testBasic();
 
 public:

  testPipeUnit();
  ~testPipeUnit();

  int applyTest(const int);     

};

#endif
