/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BremBlockGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_BremBlockGenerator_h
#define xraySystem_BremBlockGenerator_h

class Simulation;

namespace setVariable
{
  
/*!
  \class BremBlockGenerator
  \version 1.0
  \author S. Ansell
  \date October 2019
  \brief Bremsstralung Collimator unit  builder
*/

class BremBlockGenerator 
{
 private:

  bool centFlag;              ///< centre flag
  double radius;              ///< Main radius [-ve to use square]
  double width;               ///< Optional width
  double height;              ///< Optional height

  double holeXStep;            ///< X-offset of hole
  double holeZStep;            ///< Z-offset of hole
  
  double holeAWidth;           ///< Front width of hole
  double holeAHeight;          ///< Front height of hole
  double holeMidDist;          ///< Mid distance width of hole
  double holeMidWidth;         ///< Mid width of hole
  double holeMidHeight;        ///< Mid height of hole
  double holeBWidth;           ///< Back width of hole
  double holeBHeight;          ///< Back height of hole
  
  std::string voidMat;                ///< void material
  std::string mainMat;                ///< main material

  
 public:

  BremBlockGenerator();
  BremBlockGenerator(const BremBlockGenerator&);
  BremBlockGenerator& operator=(const BremBlockGenerator&);
  ~BremBlockGenerator();

  void centre() { centFlag=1; }
  void setMaterial(const std::string&,const std::string&);
  void setRadius(const double);
  void setCube(const double,const double);
  void setHoleXY(const double,const double);
  void setAperature(const double,const double,const double,
		    const double,const double,
		    const double,const double);
  
  void generateBlock(FuncDataBase&,const std::string&,
		     const double,const double) const;

};

}

#endif
 
