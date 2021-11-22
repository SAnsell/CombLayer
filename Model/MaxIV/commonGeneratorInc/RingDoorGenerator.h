/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/RingDoorGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_RingDoorGenerator_h
#define setVariable_RingDoorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class RingDoorGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief RingDoorGenerator for variables
*/

class RingDoorGenerator
{
 private:

  double innerHeight;           ///< height of inner door
  double innerWidth;            ///< width of inner door
  double innerThick;            ///< thickness of first door
  
  double outerHeight;           ///< height of outer door 
  double outerWidth;            ///< width of outer door

  double gapSpace;              ///< clearance gaps
  double innerTopGap;           ///< clearance gap top linal
  double outerTopGap;           ///< clearance gaps

  double tubeRadius;            ///< Radius of top tubes
  double tubeXStep;             ///< X of top tubes
  double tubeZStep;             ///< Z of top tubes

  double underStepHeight;       ///< height of gap
  double underStepWidth;        ///< Width of under gap
  double underStepXSep;         ///< X separation of steps

  
  std::string underAMat;        ///< material for lift point (left)
  std::string underBMat;        ///< material for lift point (right)
  std::string tubeMat;          ///< base material
  std::string doorMat;          ///< door material
    

 public:

  RingDoorGenerator();
  explicit RingDoorGenerator(const bool);
  RingDoorGenerator(const RingDoorGenerator&);
  RingDoorGenerator& operator=(const RingDoorGenerator&);
  ~RingDoorGenerator();


  void setInner(const double,const double,const double);
  void setOuter(const double,const double);
  /// Set the gap
  void setGap(const double G) { gapSpace=G; }
  /// Set the material
  void setDoorMat(const std::string& M) { doorMat=M; }

  void generateDoor(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
 
