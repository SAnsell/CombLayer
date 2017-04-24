/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essInc/RectPipeGenerator.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef setVariable_RectPipeGenerator_h
#define setVariable_RectPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class RectPipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief RectPipeGenerator for variables
*/

class RectPipeGenerator
{
 private:


  double pipeWidth;           ///< main width
  double pipeHeight;          ///< main height
  double pipeThick;           ///< main radius
  double flangeWidth;         ///< flange Radius (>radius)
  double flangeHeight;        ///< flange Radius (>radius)
  double flangeLen;           ///< flange Length
  double windowWidth;         ///< window radius (radius > WR > flangeR)
  double windowHeight;        ///< window radius (radius > WR > flangeR)
  double windowThick;         ///< window thickness
  
  std::string pipeMat;        ///< Primary default mat
  std::string windowMat;      ///< window mat
  std::string voidMat;        ///< void mat
    
 public:

  RectPipeGenerator();
  RectPipeGenerator(const RectPipeGenerator&);
  RectPipeGenerator& operator=(const RectPipeGenerator&);
  ~RectPipeGenerator();

  void setPipe(const double,const double,const double);
  void setWindow(const double,const double,const double);
  void setFlange(const double,const double,const double);
  /// set pipe material
  void setMat(const std::string& M) { pipeMat=M; }
  /// set collet material
  void setWindowMat(const std::string& M) { windowMat=M; }
  
  void generatePipe(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
