/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/FlangeMountGenerator.h
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
#ifndef setVariable_FlangeMountGenerator_h
#define setVariable_FlangeMountGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class FlangeMountGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief FlangeMountGenerator for variables
*/

class FlangeMountGenerator
{
 private:

  
  double plateRadius;            ///< Port inner radius (opening)
  double plateThick;             ///< Port outer ring

  double threadRadius;           ///< Thread radius
  double threadLength;           ///< Thread length
  
  double bladeLift;             ///< Height of blade up
  double bladeXYAngle;          ///< Angle of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeWidth;            ///< moving blade radius
  double bladeHeight;           ///< moving blade radius
  

  std::string threadMat;        ///< Thead material
  std::string bladeMat;         ///< blade material
  std::string plateMat;         ///< Pipe material
    

 public:

  FlangeMountGenerator();
  FlangeMountGenerator(const FlangeMountGenerator&);
  FlangeMountGenerator& operator=(const FlangeMountGenerator&);
  ~FlangeMountGenerator();

  template<typename CF> void setCF();
  
  // R/T/mat
  void setPlate(const double,const double,const std::string&);
  // W/H/T/Ang/Mat
  void setBlade(const double,const double,
		const double,const double,
		const std::string&);
  void setThread(const double,const double,
		 const std::string&);

  /// set the lift
  void setLift(const double L) { bladeLift=L; }
  
  void setMaterial(const std::string&,const std::string&,const std::string&);
  
  void generateMount(FuncDataBase&,const std::string&,
		     const int) const;

};

}

#endif
 
