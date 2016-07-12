/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essInc/PinHoleGenerator.h
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
#ifndef setVariable_PinHoleGenerator_h
#define setVariable_PinHoleGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PinHoleGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PinHoleGenerator for variables
*/

struct holeInfo
{
  int shape;         ///< Shape [number until enum]
  double radius;     ///< Radius
  double xradius;    ///< Radius [if needed]
  double angCent;    ///< Angle centre
  double angOff;     ///< offset
  double radStep;    ///< out setp
};
 
  
class PinHoleGenerator
{
 private:

  std::string defMat;   ///< Default material
  /// Blade thicknesses
  std::vector<holeInfo> HData;

  
 public:

  PinHoleGenerator();
  PinHoleGenerator(const PinHoleGenerator&);
  PinHoleGenerator& operator=(const PinHoleGenerator&);
  ~PinHoleGenerator();


  
  void resetHoles() { HData.clear(); }
  void addHole(const std::string,const double,const double,
	       const double,const double);
  
  
  void generatePinHole(FuncDataBase&,const std::string&,
		       const double)  const;
};

}

#endif
 
