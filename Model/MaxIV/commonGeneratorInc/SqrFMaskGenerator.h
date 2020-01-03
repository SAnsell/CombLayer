/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/SqrFMaskGenerator.h
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
#ifndef setVariable_SqrFMaskGenerator_h
#define setVariable_SqrFMaskGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SqrFMaskGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief SqrFMaskGenerator for variables
*/

class SqrFMaskGenerator :
  public CollGenerator
{
 private:

  double width;           ///< Outer width
  double height;          ///< Outer height

  double aFInRadius;      ///< A flange Inner radius
  double aFOutRadius;     ///< A flange Outer radius
  double aFLength;        ///< A flange length
  double bFInRadius;      ///< B flange Inner radius
  double bFOutRadius;     ///< B flange Outer radius
  double bFLength;        ///< B flange length

  double pipeRadius;      ///< Pipe radius
  double pipeXWidth;      ///< Pipe x-width
  double pipeZDepth;      ///< Pipe distance below beam
  double pipeYStart;      ///< Pipe yStart point [from front]
  double pipeYStep;       ///< Pipe yStep
  
  std::string flangeMat;         ///< main material    
  std::string waterMat;          ///< coolant material
  
 public:

  SqrFMaskGenerator();
  SqrFMaskGenerator(const SqrFMaskGenerator&);
  SqrFMaskGenerator& operator=(const SqrFMaskGenerator&);
  virtual ~SqrFMaskGenerator();
  
  template<typename CF> void setCF();
  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();

  void setFlangeMat(const std::string&);
  
  virtual void generateColl(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
