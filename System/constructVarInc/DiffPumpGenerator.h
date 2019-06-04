/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/DiffPumpGenerator.h
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
#ifndef setVariable_DiffPumpGenerator_h
#define setVariable_DiffPumpGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DiffPumpGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief DiffPumpGenerator for variables
*/

class DiffPumpGenerator
{
 private:
    
  double width;                 ///< Width
  double height;                ///< Height
  double apertureWidth;         ///< Horizontal dimension of aperture
  double apertureHeight;        ///< Vertical dimension of aperture

  double flangeRadius;        ///< Flange radius
  double flangeThick;         ///< Flange thickness
  double flangeVoidWidth;     ///< Flange void width
  double flangeVoidHeight;    ///< Flange void height
  double flangeVoidThick;     ///< Flange void thickness

  double magnetWidth;         ///< Magnet width
  double magnetLength;        ///< Magnet length
  double magnetThick;         ///< Magnet thickness
  double magnetGap;           ///< Thickness of gap between pump body and magnet

  std::string mat;                   ///< Material
  std::string magnetMat;             ///< Magnet material
  std::string flangeMat;             ///< Flange material

 public:

  DiffPumpGenerator();
  DiffPumpGenerator(const DiffPumpGenerator&);
  DiffPumpGenerator& operator=(const DiffPumpGenerator&);
  ~DiffPumpGenerator();

  template<typename CF> void setCF();
    
  void generatePump(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
 
