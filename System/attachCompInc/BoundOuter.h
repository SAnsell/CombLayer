/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BoundOuter.h
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
#ifndef attachSystem_BoundOuter_h
#define attachSystem_BoundOuter_h

class Simulation;

namespace attachSystem
{
  
/*!
  \class BoundOuter
  \version 1.0
  \author S. Ansell
  \date October 2016
  \brief BoundOuter unit  
  
  Produces a bound surface inner/outer
*/

class BoundOuter 
{
 protected:
  
  int setFlag;                  ///< Structures set
  HeadRule innerStruct;         ///< Front face
  HeadRule outerStruct;         ///< Back face

  
 public:

  BoundOuter();
  BoundOuter(const BoundOuter&);
  BoundOuter& operator=(const BoundOuter&);
  virtual ~BoundOuter();

  void setInnerSurf(const int);
  /// Normal object insert path
  void setInner(const HeadRule&);
  void setInnerExclude(const HeadRule&);
  void setOuter(const HeadRule&);
  void setOuterExclude(const HeadRule&);

};

}

#endif
 
