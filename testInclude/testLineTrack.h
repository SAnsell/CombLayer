/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   testInclude/testLineTrack.h
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
#ifndef testLineTrack_h
#define testLineTrack_h 

namespace ModelSupport
{
  class LineTrack;
}
/*!
  \class testLineTrack
  \brief Tests the class Object
  \author S. Ansell
  \date November 2010
  \version 1.0

  Test the processing of object component
*/

class testLineTrack
{
private:
  
  Simulation ASim;           ///< Simulation model

  void initSim();
  void createSurfaces();
  void createObjects();

  int checkResult(const ModelSupport::LineTrack&,const int,
		  const double) const;

  //Tests 
  int testLine();
  

public:
  
  testLineTrack();
  ~testLineTrack();
  
  int applyTest(const int);       

};

#endif
