/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/BellowGenerator.h
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
#ifndef setVariable_BellowGenerator_h
#define setVariable_BellowGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BellowGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief BellowGenerator for variables
*/

class BellowGenerator
{
 private:

  double pipeRadius;             ///< main radius
  double pipeThick;              ///< metal thickness
  double flangeARadius;          ///< flange Radius (>radius)
  double flangeALen;             ///< flange Length
  double flangeBRadius;          ///< flange Radius (>radius)
  double flangeBLen;             ///< flange Length

  double bellowStep;             ///< bellow step from flange
  double bellowThick;             ///< bellow step from flange
  
  std::string pipeMat;          ///< Primary default mat
  std::string voidMat;          ///< void mat
  std::string bellowMat;        ///< Primary bellow
    
 public:

  BellowGenerator();
  BellowGenerator(const BellowGenerator&);
  BellowGenerator& operator=(const BellowGenerator&);
  ~BellowGenerator();

  void setPipe(const double,const double,const double,const double);
  void setFlange(const double,const double);
  void setFlangePair(const double,const double,const double,const double);
  /// set pipe material
  void setMat(const std::string& M,const double);

  
  void generateBellow(FuncDataBase&,const std::string&,
		    const double,const double) const;

};

}

#endif
 
