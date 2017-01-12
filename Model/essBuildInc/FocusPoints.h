/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/FocusPoints.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef essSystem_FocusPoints_h
#define essSystem_FocusPoints_h

class Simulation;

namespace essSystem
{
  class MidWaterDivider;
  class EdgeWater;

/*!
  \class FocusPoints
  \author Stuart Ansell 
  \version 1.0
  \date January 2017
  \brief Focus point system for moderator views  
*/

class FocusPoints :
  public attachSystem::FixedOffset
{
 private:

  double forwardDist;         ///< Forward displacement
  double backDist;            ///< back displacement
  double leftWidth;           ///< left displacement
  double rightWidth;          ///< right displacement
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createLinks();
  
 public:
  
  FocusPoints(const std::string&);
  FocusPoints(const FocusPoints&);
  FocusPoints& operator=(const FocusPoints&);
  virtual FocusPoints* clone() const;
  virtual ~FocusPoints();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
};

}

#endif
 
