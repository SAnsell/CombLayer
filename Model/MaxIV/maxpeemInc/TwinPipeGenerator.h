/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/TwinPipeGenerator.h
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
#ifndef setVariable_TwinPipeGenerator_h
#define setVariable_TwinPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TwinPipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief TwinPipeGenerator for variables
*/

class TwinPipeGenerator 
{
 private:
  
  double pipeAXStep;             ///< x step 
  double pipeAZStep;             ///< Z step
  double pipeAXYAngle;             ///< main radius
  double pipeAZAngle;             ///< main radius
  double pipeARadius;             ///< main radius
  double pipeAThick;              ///< metal thickness
  

  double pipeBXStep;             ///< x step 
  double pipeBZStep;             ///< Z step
  double pipeBXYAngle;            ///< main radius
  double pipeBZAngle;             ///< main radius
  double pipeBRadius;             ///< main radius
  double pipeBThick;              ///< metal thickness

  double flangeCJRadius;          ///< flange Radius (>radius)
  double flangeCJLength;          ///< flange Length
  double flangeARadius;           ///< flange Radius (>radius)
  double flangeALength;           ///< flange Length
  double flangeBRadius;           ///< flange Radius (>radius)
  double flangeBLength;           ///< flange Length
  
  std::string pipeMat;          ///< Primary default mat
  std::string voidMat;          ///< void mat

 public:

  TwinPipeGenerator();
  TwinPipeGenerator(const TwinPipeGenerator&);
  TwinPipeGenerator& operator=(const TwinPipeGenerator&);
  ~TwinPipeGenerator();

  template<typename CF> void setCF();

  template<typename CF> void setAPipeCF();
  template<typename CF> void setBPipeCF();
  template<typename CF> void setJoinFlangeCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();

  void setAPos(const double,const double);
  void setBPos(const double,const double);
  void setXYAngle(const double,const double);
  
  void setMat(const std::string&);

  
  void generateTwin(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
