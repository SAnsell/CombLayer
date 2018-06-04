/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/JawFlangeGenerator.h
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
#ifndef setVariable_JawFlangeGenerator_h
#define setVariable_JawFlangeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class JawFlangeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief JawFlangeGenerator for variables
*/

class JawFlangeGenerator
{
 private:

  
  double radius;                ///< Void radius
  double length;                ///< void length
  
  double jawStep;               ///< Step along beam direction
  double jawOpen;               ///< gap between jaws
  double jawWidth;              ///< Width of both jaws  (xbeam)
  double jawHeight;             ///< height of both jaws (above beam)
  double jawThick;              ///< thickness [in beam]
  
  std::string jawMat;           ///< Jaw material
   
 public:

  JawFlangeGenerator();
  JawFlangeGenerator(const JawFlangeGenerator&);
  JawFlangeGenerator& operator=(const JawFlangeGenerator&);
  ~JawFlangeGenerator();

  void setSlits(const double,const double,const double,
		const std::string&);
  
  void generateFlange(FuncDataBase&,const std::string&) const;

};

}

#endif
 
