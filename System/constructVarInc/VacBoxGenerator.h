/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/VacBoxGenerator.h
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
#ifndef setVariable_VacBoxGenerator_h
#define setVariable_VacBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class VacBoxGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief VacBoxGenerator for variables
*/

class VacBoxGenerator
{
 private:

  double wallThick;          ///< Wall thick [generic]

  double portWallThick;     ///< Flange wall thickness
  double portTubeLength;    ///< Port tube
  double portTubeRadius;    ///< Port tube length
  
  double flangeLen;          ///< Flange length
  double flangeRadius;       ///< Flange radius
    
  std::string voidMat;          ///< Primary default mat
  std::string wallMat;          ///< Primary default mat

 public:

  VacBoxGenerator();
  VacBoxGenerator(const VacBoxGenerator&);
  VacBoxGenerator& operator=(const VacBoxGenerator&);
  ~VacBoxGenerator();


  void setPort(const double,const double,const double);
  void setFlange(const double,const double);
  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set pipe material
  void setMat(const std::string& M) { wallMat=M; }

  
  void generateBox(FuncDataBase&,const std::string&,
		   const double,const double,const double,
		   const double,const double) const;

};

}

#endif
 
