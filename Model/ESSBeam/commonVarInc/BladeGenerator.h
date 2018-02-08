/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/BladeGenerator.h
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
#ifndef setVariable_BladeGenerator_h
#define setVariable_BladeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BladeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BladeGenerator for variables
*/

class BladeGenerator
{
 private:
     
  double gap;             ///< inner height

  /// Blade thicknesses
  std::vector<double> thick;
  std::vector<double> innerThick;  ///< Inner thickness [if different]
  std::string innerMat;     ///< inner material
  std::string outerMat;     ///< Outer material
  
  std::vector<std::vector<double>> CentreAngle; ///< Centre position of gap
  std::vector<std::vector<double>> OpenAngle;   ///< Opening of gap
  
  
 public:

  BladeGenerator();
  BladeGenerator(const BladeGenerator&);
  BladeGenerator& operator=(const BladeGenerator&);
  ~BladeGenerator();

  /// set collet material
  void setMaterials(const std::string&,const std::string&);
  void setGap(const double G) { gap=G; }  ///< set gap value
  void setThick(const std::vector<double>&);
  void setInnerThick(const std::vector<double>&);  
  void resetPhase();
  void setPhase(const size_t,const std::vector<double>&,
		const std::vector<double>&);
  void addPhase(const std::vector<double>&,
		const std::vector<double>&);
  
  
  void generateBlades(FuncDataBase&,const std::string&,
		      const double,const double,const double)  const;
};

}

#endif
 
