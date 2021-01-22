/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/GateValveGenerator.h
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
#ifndef setVariable_GateValveGenerator_h
#define setVariable_GateValveGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class GateValveGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief GateValveGenerator for variables
*/

class GateValveGenerator
{
 private:

  double length;                ///< Void length
  double radius;                ///< Radius if round
  double width;                 ///< Void width (full)
  double height;                ///< height 
  double depth;                 ///< depth
  
  double wallThick;             ///< Wall thickness

  double portARadius;            ///< Port inner radius (opening)
  double portAThick;             ///< Port outer ring
  double portALen;               ///< Forward step of port

  double portBRadius;            ///< Port inner radius (opening)
  double portBThick;             ///< Port outer ring
  double portBLen;               ///< Forward step of port
  
  double bladeLift;             ///< Height of blade up
  double bladeThick;            ///< moving blade thickness
  double bladeRadius;           ///< moving blade radius

  double liftWidth;             ///< Lifting width
  double liftHeight;            ///< Height of lifting space

  std::string voidMat;          ///< Void material
  std::string bladeMat;         ///< Void material
  std::string wallMat;          ///< Pipe material

 public:

  GateValveGenerator();
  GateValveGenerator(const GateValveGenerator&);
  GateValveGenerator& operator=(const GateValveGenerator&);
  ~GateValveGenerator();

  template<typename CF> void setCylCF();
  template<typename CF> void setCubeCF();
  template<typename CF> void setAPortCF();
  template<typename CF> void setBPortCF();

  template<typename innerCF,typename outerCF> void setPortPairCF();

  
  /// set wall thickness
  void setWallThick(const double T) { wallThick=T; }
  /// set blade thickness
  void setBladeThick(const double T) { bladeThick=T; }

  /// set total thickness
  void setLength(const double L) { length=L; }
  void setOuter(const double,const double,
		const double,const double);
  void setPort(const double,const double,const double);
  void setAPort(const double,const double,const double);
  void setBPort(const double,const double,const double);
  
  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set wall material
  void setWallMat(const std::string& M) { wallMat=M; }
  /// set wall material
  void setBladeMat(const std::string& M) { bladeMat=M; }
  
  void generateValve(FuncDataBase&,const std::string&,
		     const double,const int) const;

};

}

#endif
 
