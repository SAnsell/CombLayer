/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testLineIntersect.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef testLineIntersect_h
#define testLineIntersect_h 

/*!
  \class testLineIntersect
  \brief Tests the class Object
  \author S. Ansell
  \date November 2024
  \version 1.0

  Test the processing of object component
*/

class testLineIntersect
{
private:
  
  SimMCNP ASim;           ///< Simulation model

  void initSim();
  void createSurfaces();
  void createObjects();

  //Tests 
  int testInterPoint();
  

public:
  
  testLineIntersect();
  ~testLineIntersect();
  
  int applyTest(const int);       

};

#endif
