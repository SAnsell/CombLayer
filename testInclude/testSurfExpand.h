/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testSurfExpand.h
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
#ifndef testSurfExpand_h
#define testSurfExpand_h 

/*!
  \class testSurfExpand
  \brief Tests the SurfExpand class
  \author S. Ansell
  \date December 2009
  \version 1.0
  
  Test of the surf Expand system
*/

class testSurfExpand 
{
 private:
  
  /// Storage for test surface pair and fraction
  typedef boost::tuple<std::string,std::string,double> TTYPE;

  template<typename SurfTYPE>  
    int procSurface(const std::vector<TTYPE>&) const;

  //Tests 
  //  int testCreateWrap();
  int testCylinder();
  int testPlane();
 
 public:

  testSurfExpand();
  ~testSurfExpand();

  int applyTest(const int);     
};

#endif
