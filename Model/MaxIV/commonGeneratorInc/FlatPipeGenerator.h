/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/FlatPipeGenerator.h
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
#ifndef setVariable_FlatPipeGenerator_h
#define setVariable_FlatPipeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class FlatPipeGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief FlatPipeGenerator for variables
*/

class FlatPipeGenerator 
{
 private:

  double width;                  ///< void width [inner]
  double height;                 ///< void height [inner]
  double length;                 ///< void length [total]
  
  double wallThick;              ///< pipe thickness

  double flangeARadius;          ///< Joining Flange radius 
  double flangeALength;          ///< Joining Flange length

  double flangeBRadius;          ///< Joining Flange radius 
  double flangeBLength;          ///< Joining Flange length
    
  std::string voidMat;                   ///< Void material
  std::string wallMat;                     ///< Pipe material

  
 public:

  FlatPipeGenerator();
  FlatPipeGenerator(const FlatPipeGenerator&);
  FlatPipeGenerator& operator=(const FlatPipeGenerator&);
  virtual ~FlatPipeGenerator();

  template<typename CF> void setAFlangeCF();
  template<typename CF> void setBFlangeCF();
  
  void generateFlat(FuncDataBase&,const std::string&,
		    const double) const;

};

}

#endif
 
