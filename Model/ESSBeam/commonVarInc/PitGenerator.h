/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/PitGenerator.h
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
#ifndef setVariable_PitGenerator_h
#define setVariable_PitGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class PitGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief PitGenerator for variables
*/

class PitGenerator
{
 private:
     
  double feHeight;             ///< inner height
  double feDepth;              ///< inner depth
  double feWidth;              ///< inner widht
  double feFront;              ///< inner front
  double feBack;               ///< inner back

  double concHeight;             ///< outer height
  double concDepth;              ///< outer depth
  double concWidth;              ///< outer widht
  double concFront;              ///< outer front
  double concBack;               ///< outer back
  
  std::string feMat;           ///< Primary default mat
  std::string concMat;         ///< Secondary default mat
  std::string colletMat;       ///< collet  default mat
  
  
 public:

  PitGenerator();
  PitGenerator(const PitGenerator&);
  PitGenerator& operator=(const PitGenerator&);
  ~PitGenerator();

  /// set collet material
  void setColletMat(const std::string& M)
  { colletMat=M; }
  /// void collett
  void removeCollet() { colletMat="InValid"; }
  void setFeLayer(const double);
  void setConcLayer(const double);
  
  void generatePit(FuncDataBase&,const std::string&,
		      const double,const double,const double,const double,
		      const double)  const;
};

}

#endif
 
