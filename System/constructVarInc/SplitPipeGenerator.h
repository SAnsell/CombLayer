/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/SplitPipeGenerator.h
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
#ifndef setVariable_SplitPipeGenerator_h
#define setVariable_SplitPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SplitPipeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief SplitPipeGenerator for variables
*/

class SplitPipeGenerator
{
 private:

  double pipeRadius;             ///< main radius
  double pipeThick;              ///< metal thickness
  double flangeARadius;          ///< flange Radius (>radius)
  double flangeALen;             ///< flange Length
  double flangeBRadius;          ///< flange Radius (>radius)
  double flangeBLen;             ///< flange Length
  
  std::string pipeMat;          ///< Primary default mat
  std::string voidMat;          ///< void mat
    
 public:

  SplitPipeGenerator();
  SplitPipeGenerator(const SplitPipeGenerator&);
  SplitPipeGenerator& operator=(const SplitPipeGenerator&);
  ~SplitPipeGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();

  void setPipe(const double,const double);
  void setFlange(const double,const double);
  void setFlangePair(const double,const double,const double,const double);
  void setMat(const std::string&);

  const std::string& getPipeMat() const { return pipeMat; }
  void generatePipe(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
