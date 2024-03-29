/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/HPJawsGenerator.h
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
#ifndef setVariable_HPJawsGenerator_h
#define setVariable_HPJawsGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class HRJawGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief HPJawsGenerator for variables
*/

class HPJawsGenerator 
{
 private:
  

  double radius;           ///< main void radius
  double length;           ///< Void length 
  double sideThick;        ///< Wall thickness
  double wallThick;        ///< Front/Back Wall thickness

  double flangeInnerRadius;  ///< Port radius
  double flangeRadius;       ///< Attached back flange radius
  double flangeLength;       ///<  Attached back flange length 

  double dividerThick;       ///< Divider for jaws
  double dividerGap;         ///< Mid point divider gap [static]

  double jawFarLen;          ///< Length extending away from beam
  double jawEdgeLen;         ///< Length on cutting surface
  double jawThick;           ///< Thickness
  double jawCornerEdge;      ///< Corner cut size
  double jawCornerFar;       ///< Corner cut size
  
  std::string voidMat;       ///< void material
  std::string wallMat;       ///< main material
  std::string jawMat;        ///< main material
 
 public:

  HPJawsGenerator();
  HPJawsGenerator(const HPJawsGenerator&);
  HPJawsGenerator& operator=(const HPJawsGenerator&);
  virtual ~HPJawsGenerator();

  void setMain(const double L) { length=L; }
  template<typename T> void setPortCF();
  
  void generateJaws(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
