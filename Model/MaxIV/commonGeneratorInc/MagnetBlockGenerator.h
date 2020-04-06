/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/MagnetBlockGenerator.h
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
#ifndef setVariable_MagnetBlockGenerator_h
#define setVariable_MagnetBlockGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MagnetBlockGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief MagnetBlockGenerator for variables
*/

class MagnetBlockGenerator
{
 private:

  double blockYStep;            ///< Step forward of main block
  double aLength;               ///< first length  (x2)
  double bLength;               ///< second length (x2)
  double midLength;             ///< Mid length [full]
  double sectorAngle;           ///< Angle of sections 

  double height;                ///< full height
  double width;                 ///< width across front
  
  std::string voidMat;          ///< void material
  std::string outerMat;         ///< wall material

 public:

  MagnetBlockGenerator();
  MagnetBlockGenerator(const MagnetBlockGenerator&);
  MagnetBlockGenerator& operator=(const MagnetBlockGenerator&);
  ~MagnetBlockGenerator();


  void generateBlock(FuncDataBase&,const std::string&,
		      const double) const;

};

}

#endif
 
