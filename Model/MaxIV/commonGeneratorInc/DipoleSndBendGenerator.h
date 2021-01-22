/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenertorInc/DipoleSndBendGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef setVariable_DipoleSndBendGenerator_h
#define setVariable_DipoleSndBendGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DipoleSndBendGenerator
  \version 1.0
  \author S. Ansell
  \date January 2019
  \brief DipoleSndBendGenerator for variables
*/

class DipoleSndBendGenerator 
{
 private:

  double arcAngle;             ///< full arc angle [deg]

  double curveRadius;          ///< Radius of centre line
  double flatWidth;            ///< Step to outer surf
  double outerFlat;            ///< straight line from beam
  
  double tipHeight;            ///< tip height
  double height;               ///< Beam height
  double outerHeight;          ///< Outer height

  double wallThick;            ///< Wall thickness
  
  std::string voidMat;         ///< void material
  std::string wallMat;         ///< wall material
  std::string outerMat;        ///< inner material

 public:

  DipoleSndBendGenerator();
  DipoleSndBendGenerator(const DipoleSndBendGenerator&);
  DipoleSndBendGenerator& operator=(const DipoleSndBendGenerator&);
  virtual ~DipoleSndBendGenerator();
  
  
  virtual void generatePipe(FuncDataBase&,const std::string&,
			    const double,const double=0.0) const;

};

}

#endif
 
