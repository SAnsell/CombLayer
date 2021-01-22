/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/GaugeGenerator.h
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
#ifndef setVariable_GaugeGenerator_h
#define setVariable_GaugeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class GaugeGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief GaugeGenerator for variables
*/

class GaugeGenerator 
{
 private:
  
  double yRadius;               ///< radiu of Y (Beam) port
  double xRadius;               ///< radius of x port 

  double wallThick;             ///< pipe thickness
  
  double length;                ///< full to flange length (Y axis)

  double sideLength;            ///< beam to flange length
  
  double flangeXRadius;         ///< Joining Flange radius
  double flangeYRadius;         ///< Joining Flange radius

  double flangeXLength;         ///< Joining Flange radius
  double flangeYLength;         ///< Joining Flange radius
  
  double plateThick;            ///< Joining Flange radius

  std::string voidMat;          ///< void material
  std::string wallMat;          ///< main material
  std::string plateMat;         ///< plate material
 
 public:

  GaugeGenerator();
  GaugeGenerator(const GaugeGenerator&);
  GaugeGenerator& operator=(const GaugeGenerator&);
  virtual ~GaugeGenerator();

  template<typename T> void setCF();
  template<typename T> void setFlangeCF();
  template<typename T> void setSidePortCF(const double);
  void setPlateThick(const double T,const std::string& M)
  { plateThick=T; plateMat=M; }
  void setLength(const double);
  
  void generateGauge(FuncDataBase&,
		     const std::string&,
		     const double,
		     const double) const;

};

}

#endif
 
