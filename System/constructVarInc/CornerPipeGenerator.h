/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/CornerPipeGenerator.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef setVariable_CornerPipeGenerator_h
#define setVariable_CornerPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CornerPipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief CornerPipeGenerator for variables
*/

class CornerPipeGenerator
{
 private:
  
  double cornerRadius;          ///< void corner [inner]
  double height;                ///< void height [inner]
  double width;                 ///< void width [inner]

  double length;                ///< void length [total]

  double wallThick;               ///< pipe thickness

  double flangeARadius;          ///< Joining Flange radius [-ve for rect]
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeBLength;          ///< Joining Flange length

  std::string voidMat;                   ///< Void material
  std::string wallMat;                   ///< Pipe material
  std::string flangeMat;                 ///< Flange material

  int outerVoid;                 ///< Flag to build the outer void cell between 


 public:

  CornerPipeGenerator();
  CornerPipeGenerator(const CornerPipeGenerator&);
  CornerPipeGenerator& operator=(const CornerPipeGenerator&);
  ~CornerPipeGenerator();

  void setPipe(const double,const double,const double,const double);
  void setFlange(const double,const double);
  void setAFlange(const double,const double);
  void setBFlange(const double,const double);
  void setFlangePair(const double,const double,const double,const double);

  template<typename CF> void setCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();

  /// setter for material name
  void setMat(const std::string&,const std::string&);

  /// set out vied
  void setOuterVoid(const int val =1) { outerVoid = val; }

  void generatePipe(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
