/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/PreDipoleGenerator.h
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
#ifndef setVariable_PreDipoleGenerator_h
#define setVariable_PreDipoleGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PreDipoleGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief PreDipoleGenerator for variables
*/

class PreDipoleGenerator 
{
 private:

  
  double length;                 ///< frame length
  double radius;                 ///< Primary radius

  double straightLength;         ///< straight length

  double wallThick;              ///< wall thickness

  double electronRadius;          ///< radius of electron offset
  double electronAngle;           ///< Angle of electron offset

  double flangeARadius;           ///< flange radius
  double flangeALength;           ///< flange length

  double flangeBRadius;           ///< back flange radius
  double flangeBLength;           ///< back flange length

  std::string voidMat;                    ///< void material
  std::string wallMat;                    ///< wall material
  std::string flangeMat;                  ///< flange port material

 public:

  PreDipoleGenerator();
  PreDipoleGenerator(const PreDipoleGenerator&);
  PreDipoleGenerator& operator=(const PreDipoleGenerator&);
  virtual ~PreDipoleGenerator();
  
  
  virtual void generatePipe(FuncDataBase&,const std::string&) const;

};

}

#endif
 
