/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/VacuumBoxGenerator.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell & Konstantin Batkov
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
#ifndef setVariable_VacuumBoxGenerator_h
#define setVariable_VacuumBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class VacuumBoxGenerator
  \version 1.1
  \author S. Ansell and K. Batkov
  \date Match 2026
  \brief Variable generator for the VacuumBox class
*/

class VacuumBoxGenerator
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

  double flangeARadius;       ///< Flange Radius
  double flangeALength;          ///< Flange length
  double flangeBRadius;       ///< Flange radius
  double flangeBLength;          ///< Flange length

  std::string voidMat;          ///< Void material
  std::string wallMat;          ///< Wall material
  std::string pipeMat;          ///< Pipe (port/flangeA/B) material

 public:

  VacuumBoxGenerator();
  VacuumBoxGenerator(const VacuumBoxGenerator&);
  VacuumBoxGenerator& operator=(const VacuumBoxGenerator&);
  virtual ~VacuumBoxGenerator();


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

  void generateBox(FuncDataBase&,const std::string&,
		   const double,const double,
		   const double,const double) const;

};

}

#endif
