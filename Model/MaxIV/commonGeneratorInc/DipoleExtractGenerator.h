/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/DipoleExtractGenerator.h
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
#ifndef setVariable_DipoleExtractGenerator_h
#define setVariable_DipoleExtractGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DipoleExtractGenerator
  \version 1.0
  \author S. Ansell
  \date January 2019
  \brief DipoleExtractGenerator for variables
*/

class DipoleExtractGenerator 
{
 private:

  double length;                ///< frame length
  double width;                 ///< width of inner frame
  double height;                ///< Inner height [straight]
  double wideHeight;            ///< Height at end of triangle
  double wideWidth;             ///< Width at which the triangle stops
  
  double exitLength;            ///< Cut in exit channel

  double wallThick;             ///< Wall thickness
  double edgeThick;             ///< Edge Wall thickness
  
  std::string voidMat;                  ///< void material
  std::string wallMat;                  ///< wall material
  std::string outerMat;                 ///< inner material
  
 public:

  DipoleExtractGenerator();
  DipoleExtractGenerator(const DipoleExtractGenerator&);
  DipoleExtractGenerator& operator=(const DipoleExtractGenerator&);
  virtual ~DipoleExtractGenerator();

  /// accessor to length
  void setLength(const double D) { length=D; }
  
  virtual void generatePipe(FuncDataBase&,const std::string&,
			    const double) const;

};

}

#endif
 
