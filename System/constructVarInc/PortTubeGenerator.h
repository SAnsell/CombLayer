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
#ifndef setVariable_PortTubeGenerator_h
#define setVariable_PortTubeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PortTubeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PortTubeGenerator for variables
*/

class PortTubeGenerator
{
 private:

  double wallThick;          ///< Wall thick [generic]

  double portAXStep;         ///< Step on in-port
  double portAZStep;         ///< Step of in-port
  double portAWallThick;     ///< Flange wall thickness
  double portATubeLength;    ///< Port tube
  double portATubeRadius;    ///< Port tube length

  double portBXStep;         ///< Step on out-port
  double portBZStep;         ///< Step on out-port
  double portBWallThick;     ///< Flange wall thickness
  double portBTubeLength;    ///< Port tube
  double portBTubeRadius;    ///< Port tube length
  
  double flangeALen;          ///< Flange length
  double flangeARadius;       ///< Flange radius
  double flangeBLen;          ///< Flange length
  double flangeBRadius;       ///< Flange radius
    
  std::string voidMat;          ///< Primary default mat
  std::string wallMat;          ///< Primary default mat

 public:

  PortTubeGenerator();
  PortTubeGenerator(const PortTubeGenerator&);
  PortTubeGenerator& operator=(const PortTubeGenerator&);
  ~PortTubeGenerator();


  template<typename CF> void setCF();
  template<typename CF> void setAPortCF();
  template<typename CF> void setBPortCF();

  /// set wall thickness  
  void setWallThick(const double T) { wallThick=T; }

  void setPortLength(const double,const double);
  void setPort(const double,const double,const double);
  void setAPort(const double,const double,const double);
  void setBPort(const double,const double,const double);
  void setAPortOffset(const double,const double);
  void setBPortOffset(const double,const double);
  void setAFlange(const double,const double);
  void setBFlange(const double,const double);
  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set wall material
  void setMat(const std::string& M) { wallMat=M; }

  
  void generateTube(FuncDataBase&,const std::string&,
		   const double,const double,const double) const;

};

}

#endif
 
