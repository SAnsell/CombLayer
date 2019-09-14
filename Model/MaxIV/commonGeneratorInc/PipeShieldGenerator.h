/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/PipeShieldGenerator.h
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
#ifndef setVariable_PipeShieldGenerator_h
#define setVariable_PipeShieldGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PipeShieldGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PipeShieldGenerator for variables
*/

class PipeShieldGenerator
{
 private:

  double height;                 ///< thickness of collimator
  double width;                  ///< main width
  double length;                 ///< thickness of collimator
  double clearGap;               ///< clearance gap
  double wallThick;              ///< wall thickness

  double wingThick;              ///< Extra back wings
  
  std::string mat;                       ///< material
  std::string wallMat;                   ///< wall material
  std::string wingMat;                   ///< Wing material

 public:

  PipeShieldGenerator();
  PipeShieldGenerator(const PipeShieldGenerator&);
  PipeShieldGenerator& operator=(const PipeShieldGenerator&);
  ~PipeShieldGenerator();

  void setPlate(const double,const double,const double);
  void setWall(const double W,const double WT)
     { wallThick=W; wingThick=WT; }
  void setMaterial(const std::string&,const std::string&,
		   const std::string&);

  void generateShield(FuncDataBase&,const std::string&,
		      const double,const double) const;

};

}

#endif
 
