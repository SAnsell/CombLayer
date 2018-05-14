/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/PipeGenerator.h
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
#ifndef setVariable_PipeGenerator_h
#define setVariable_PipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PipeGenerator for variables
*/

class PipeGenerator
{
 private:

  int pipeType;                 ///< type [0 for round / 1 for square]
  double pipeRadius;            ///< main radius
  double pipeHeight;            ///< main height
  double pipeWidth;             ///< main width
  double pipeThick;             ///< metal thickness
  double claddingThick;         ///< cladding radius
  double flangeARadius;          ///< flange Radius (>radius)
  double flangeALen;             ///< flange Length
  double flangeBRadius;          ///< flange Radius (>radius)
  double flangeBLen;             ///< flange Length
  double windowRadius;          ///< window radius (radius > WR > flangeR)
  double windowThick;           ///< window thickness
  
  std::string pipeMat;          ///< Primary default mat
  std::string frontWindowMat;   ///< window mat
  std::string backWindowMat;    ///< window mat
  std::string voidMat;          ///< void mat
  std::string claddingMat;      ///< Primary default mat
    
 public:

  PipeGenerator();
  PipeGenerator(const PipeGenerator&);
  PipeGenerator& operator=(const PipeGenerator&);
  ~PipeGenerator();

  void setPipe(const double,const double);
  void setRectPipe(const double,const double,const double);
  void setWindow(const double,const double);
  void setFlange(const double,const double);
  void setFlangePair(const double,const double,const double,const double);
  /// set pipe material

  template<typename CF> void setCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();


  /// setter for material name
  void setMat(const std::string& M) { pipeMat=M; }
  void setWindowMat(const std::string&);
  void setWindowMat(const std::string&,const std::string&);
  void setCladding(const double,const std::string&);
  
  void generatePipe(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
