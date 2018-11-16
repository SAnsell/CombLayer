/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testGenerateSurf.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef testGenerateSurf_h
#define testGenerateSurf_h 

/*!
  \class testGenerateSurf
  \brief Tests the geneateSurf functions
  \author S. Ansell
  \date October 2018
  \version 1.0
  
  Test of the generate Surf options
*/

class testGenerateSurf 
{
 private:
  

  //Tests 
  //  int testCreateWrap();
  int testExpandedSurf();
  int testPlane();
 
 public:

  testGenerateSurf();
  ~testGenerateSurf();

  int applyTest(const int);     
};

#endif
