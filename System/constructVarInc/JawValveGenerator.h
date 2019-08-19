/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/JawValveGenerator.h
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
#ifndef setVariable_JawValveGenerator_h
#define setVariable_JawValveGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class JawValveGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief JawValveGenerator for variables
*/

class JawValveGenerator
{
 private:

  double length;                ///< Void length
  double innerRadius;           ///< Inner radius if cyclinder
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
    
  std::string voidMat;          ///< Void material
  std::string wallMat;          ///< Pipe material

  double jawWidth;              ///< Width of both jaws  (xbeam)
  double jawHeight;             ///< height of both jaws (above beam)
  double jawThick;              ///< thickness [in beam]
  double jawGap;                ///< separation [if any]
  
  std::string jawMat;           ///< Jaw material


 public:

  JawValveGenerator();
  JawValveGenerator(const JawValveGenerator&);
  JawValveGenerator& operator=(const JawValveGenerator&);
  ~JawValveGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setAPortCF();
  template<typename CF> void setBPortCF();

  void setLength(const double L) { length=L; }
  void setRadius(const double);
  void setSlits(const double,const double,const double,const std::string&);
  void setOuter(const double,const double,const double,
		const double);
  /// set wall thickness
  void setWallThick(const double T) { wallThick=T; }

  void setPort(const double,const double,const double);
  void setAPort(const double,const double,const double);
  void setBPort(const double,const double,const double);

  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set wall material
  void setWallMat(const std::string& M) { wallMat=M; }
  
  void generateSlits(FuncDataBase&,const std::string&,
		     const double,const double,
		     const double) const;

};

}

#endif
 
