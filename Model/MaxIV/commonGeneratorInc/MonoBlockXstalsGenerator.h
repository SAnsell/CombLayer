/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoBlockXstalsGenerator.h
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
#ifndef setVariable_MonoBlockXstalsGenerator_h
#define setVariable_MonoBlockXstalsGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MonoBlockXstalsGenerator
  \version 1.0
  \author S. Ansell
  \date July 2018
  \brief MonoBlockXstalsGenerator for variables
*/

class MonoBlockXstalsGenerator
{
 private:

  double gap;               ///< Gap thickness

  double phiA;              ///< phi angle of first [rot:Y]
  double phiB;              ///< phi angle of second [rot:Y]
  
  double widthA;            ///< Radius of from centre
  double heightA;           ///< Radius of detector
  double lengthA;           ///< Outer wall thickness
  
  double widthB;            ///< Radius of from centre
  double heightB;           ///< Radius of detector
  double lengthB;           ///< Outer wall thickness

  double baseALength;       ///< Base length
  double baseAHeight;       ///< Base thickness
  double baseAWidth;        ///< Edge aligned to crystal

  double topALength;        ///< Top length
  double topAHeight;        ///< Top thickness
  double topAWidth;         ///< Edge aligned to crystal

  double baseBLength;       ///< Base length
  double baseBHeight;       ///< Base thickness
  double baseBWidth;        ///< Edge aligned to crystal

  double topBLength;        ///< Top length
  double topBHeight;        ///< Top thickness
  double topBWidth;         ///< Edge aligned to crystal
  
  std::string xtalMat;      ///< XStal material
  std::string baseMat;      ///< Base material


 public:

  MonoBlockXstalsGenerator();
  MonoBlockXstalsGenerator(const MonoBlockXstalsGenerator&);
  MonoBlockXstalsGenerator& operator=(const MonoBlockXstalsGenerator&);
  ~MonoBlockXstalsGenerator();


  void generateXstal(FuncDataBase&,const std::string&,
		     const double,const double) const;

};

}

#endif
 
