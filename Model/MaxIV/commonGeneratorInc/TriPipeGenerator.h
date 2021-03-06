/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/TriPipeGenerator.h
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
#ifndef setVariable_TriPipeGenerator_h
#define setVariable_TriPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TriPipeGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief TriPipeGenerator for variables
*/

class TriPipeGenerator 
{
 private:

  double axisXStep;                 ///< step about Z
  double axisZStep;                 ///< step about X 

  double axisXYAngle;                ///< Rotation about Z
  double axisZAngle;                 ///< Rotation about X 
 
  double frontWidth;                 ///< void width [inner]
  double frontHeight;                ///< void height [inner]
  double backWidth;                  ///< void width [inner]
  double backHeight;                 ///< void height [inner]
  
  double length;                 ///< void length [total]
  
  double wallThick;              ///< pipe thickness

  double flangeARadius;          ///< Joining Flange radius 
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius 
  double flangeBLength;          ///< Joining Flange length
    
  std::string voidMat;           ///< Void material
  std::string wallMat;           ///< Pipe material
  std::string flangeMat;         ///< Void material
  
 public:

  TriPipeGenerator();
  TriPipeGenerator(const TriPipeGenerator&);
  TriPipeGenerator& operator=(const TriPipeGenerator&);
  virtual ~TriPipeGenerator();

  void setLength(const double L) { length=L; }
  
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();
  void setXYWindow(const double,const double,const double,const double);
  
  void generateTri(FuncDataBase&,const std::string&) const;

};

}

#endif
 
