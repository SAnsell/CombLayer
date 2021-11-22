/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/VacBoxGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

  double feHeight;            ///< fe height [top only]
  double feDepth;             ///< fe depth [low only]
  double feWidth;             ///< fe width [total]
  double feFront;             ///< fe front 
  double feBack;              ///< fe back

  double portAXStep;         ///< X offset
  double portAZStep;         ///< Z offset
  double portAWallThick;     ///< Flange wall thickness
  double portATubeLength;    ///< Port tube
  double portATubeRadius;    ///< Port tube length

  double portBXStep;         ///< X offset of second flange
  double portBZStep;         ///< Z offset of second flange
  double portBXAngle;        ///< Rotation about X
  double portBZAngle;        ///< Rotation about Z
  double portBWallThick;     ///< Flange wall thickness
  double portBTubeLength;    ///< Port tube
  double portBTubeRadius;    ///< Port tube length
  
  double flangeALen;          ///< Flange length
  double flangeARadius;       ///< Flange Radius
  double flangeBLen;          ///< Flange length
  double flangeBRadius;       ///< Flange radius
    
  std::string voidMat;          ///< Primary default mat
  std::string wallMat;          ///< Primary default mat

 public:

  VacBoxGenerator();
  VacBoxGenerator(const VacBoxGenerator&);
  VacBoxGenerator& operator=(const VacBoxGenerator&);
  virtual ~VacBoxGenerator();


  /// set wall thickness
  void setWallThick(const double T) { wallThick=T; }
  void setAllThick(const double,const double,const double,
		   const double,const double);


  template<typename CF> void setCF();
  template<typename CF> void setAPortCF();
  template<typename CF> void setBPortCF();

  void setPortLength(const double,const double);
  void setPort(const double,const double,const double);

  void setAPort(const double,const double,const double);
  void setBPort(const double,const double,const double);
  void setAPortOffset(const double,const double);
  void setBPortOffset(const double,const double);
  void setBPortAngle(const double,const double);
  
  void setFlange(const double,const double);
  void setAFlange(const double,const double);
  void setBFlange(const double,const double);
  /// set void material
  void setVoidMat(const std::string& M) { voidMat=M; }
  /// set wall material
  void setMat(const std::string& M) { wallMat=M; }

  virtual void generateBox(FuncDataBase&,const std::string&,
			   const double,const double,
			   const double,const double) const;
  
};

}

#endif
 
