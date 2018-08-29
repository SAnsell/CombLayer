/****************************************************************************
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/HeavyShutterGenerator.h
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
#ifndef setVariable_HeavyShutterGenerator_h
#define setVariable_HeavyShutterGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class HeavyShutter
  \version 1.0
  \author S. Ansell
  \date February 2018
  \brief HeavyShutterGenerator for variables
*/

class HeavyShutterGenerator
{
 private:
  
  double width;                         ///< Slab width
  double height;                        ///< Slab height
  double wallThick;                     ///< Outer wall thickness [if different]
  double guideLodging;                  ///< Vertical space for guide housing
  double separatorThick;                ///< Separators thicknesses [if different]
  std::vector<double> slabThick;        ///< Slab thickness
  std::string mainMat;                  ///< Outer wall material
  std::vector<std::string> slabMat;     ///< Slab material
  
 public:
  
  HeavyShutterGenerator();
  HeavyShutterGenerator(const HeavyShutterGenerator&);
  HeavyShutterGenerator& operator=(const HeavyShutterGenerator&);
  ~HeavyShutterGenerator();
  
  void setWallThick(const double);
  void setGuideLodging(const double);
  void setSeparatorThick(const double);
  void resetSlabThick();
  void resetSlabMat();
  
  void setSlabThick(const std::vector<double>&);
  
  void setMainMaterial(const std::string&);
  void setSlabMaterial(const std::vector<std::string>&);
  
  double getLength();
  
  void generateHeavyShutter(FuncDataBase&,const std::string&,
                            const double,const double,const double) const;
};

}

#endif
