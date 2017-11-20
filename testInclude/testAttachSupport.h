/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testAttachSupport.h
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
#ifndef testAttachSupport_h
#define testAttachSupport_h 

/*!
  \class testAttachSupport
  \brief Tests the class Object
  \author S. Ansell
  \date March 2013
  \version 1.0

  Test the processing of object component
*/

class testAttachSupport
{
private:

  typedef std::shared_ptr<testSystem::simpleObj> SOTYPE;

  Simulation ASim;            ///< Simulation model [local for testing]
  std::vector<SOTYPE>  SObj;  ///<< Stack of object
  
  void initSim();
  void createSurfaces();

  //Tests 
  int testBoundaryValid();
  int testInsertComponent();

public:
  
  testAttachSupport();
  ~testAttachSupport();
  
  int applyTest(const int);       

};

#endif
