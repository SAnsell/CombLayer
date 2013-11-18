/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testMergeRule.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef testMergeRule_h
#define testMergeRule_h 

/*!
  \class testMergeRule
  \brief Tests the SurfExpand class
  \author S. Ansell
  \date December 2009
  \version 1.0
  
  Test of the surf Expand system
*/

class testMergeRule 
{
 private:

  Simulation ASim;         ///< Simulation to allow mcnpx model output

  void initSim();
  void createObjects();
  void createSurfaces();

  
  int checkSurfaceEqual(const int,const std::string&) const;
		  
  //Tests 
  int testBasicPair();
 
 public:

  testMergeRule();
  ~testMergeRule();

  int applyTest(const int);     
};

#endif
