/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testSimulation.h
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
#ifndef testSimulation_h
#define testSimulation_h 

/*!
  \class testSimulation
  \brief Tests the class Object
  \author S. Ansell
  \date November 2010
  \version 1.0

  Test the processing of object component
*/

class testSimulation
{
private:
  
  Simulation ASim;       ///< Simulation to build tests in

  void initSim();
  void createSurfaces();
  void createObjects();

  //Tests 
  int testCreateObjSurfMap();
  int testInCell();

public:
  
  testSimulation();
  ~testSimulation();
  
  int applyTest(const int);       

};

#endif
