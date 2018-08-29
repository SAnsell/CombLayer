/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/LightShutterGenerator.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef setVariable_LightShutterGenerator_h
#define setVariable_LightShutterGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class LightShutterGenerator
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief LightShutterGenerator for variables
*/

class LightShutterGenerator
{

 private:
  
  double realZStep;        ///< RealZStep
  double length;           ///< Length
  double width;            ///< Width
  double height;           ///< Height
  double wallThick;        ///< Wall thickness
  std::string mainMat;     ///< Main material
  std::string wallMat;     ///< Wall material
  
 public:
  
  LightShutterGenerator();
  LightShutterGenerator(const LightShutterGenerator&);
  LightShutterGenerator& operator=(const LightShutterGenerator&);
  ~LightShutterGenerator();
  
  void setMaterials(const std::string&,const std::string&);
  void setLength(const double);
  void setWidth(const double);
  void setHeight(const double);
  void setWallThick(const double);
  void setOpenPercentage(const double, const double);
  
  void generateLightShutter(FuncDataBase&,const std::string&,const double) const;
};

}

#endif
