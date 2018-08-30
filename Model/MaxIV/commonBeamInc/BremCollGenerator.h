/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/BremCollGenerator.h
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
#ifndef setVariable_BremCollGenerator_h
#define setVariable_BremCollGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BremCollGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BremCollGenerator for variables
*/

class BremCollGenerator
{
 private:


  double width;               ///< Width of W block
  double height;              ///< Height of W block
  double length;              ///< Main length  
  double wallThick;           ///< wall thickness of main tube

  double innerRadius;          ///< Inner radius of hole
  double flangeARadius;        ///< Joining Flange radius
  double flangeALength;        ///< Joining Flange length
  double flangeBRadius;        ///< Joining Flange radius
  double flangeBLength;        ///< Joining Flange length

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  double holeAWidth;           ///< Front width of hole
  double holeAHeight;          ///< Front height of hole
  double holeBWidth;           ///< Back width of hole
  double holeBHeight;          ///< Back height of hole

  std::string voidMat;                ///< void material
  std::string innerMat;               ///< void material
  std::string wallMat;                ///< Fe material layer
  
 public:

  BremCollGenerator();
  BremCollGenerator(const BremCollGenerator&);
  BremCollGenerator& operator=(const BremCollGenerator&);
  ~BremCollGenerator();

  template<typename CF> void setCF();
  
  void setMaterial(const std::string&,const std::string&);
  
  void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
