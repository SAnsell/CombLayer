/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/BellowGenerator.h
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

class BellowGenerator :
  public PipeGenerator
{
 private:

  double bellowStep;              ///< bellow step from flange
  double bellowThick;             ///< bellow thickness.
  
  std::string bellowMat;        ///< Primary bellow material
    
 public:

  BellowGenerator();
  BellowGenerator(const BellowGenerator&);
  BellowGenerator& operator=(const BellowGenerator&);
  ~BellowGenerator() =default;

  template<typename CF> void setCF();

  void setPipe(const double,const double,const double,const double);
  void setMat(const std::string&,const double);
  void setMat(const std::string&,const std::string&);
  
  void generateBellow(FuncDataBase&,const std::string&,
		      const double) const;

};

}

#endif
 
