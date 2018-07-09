/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVarInc/MazeGenerator.h
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
#ifndef setVariable_MazeGenerator_h
#define setVariable_MazeGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MazeGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief MazeGenerator for variables
*/

class MazeGenerator
{
 private:

  double height;           ///< height in gap
  double width;            ///< width in gap
    
  double mainOut;           ///< length toward ring centre
  double mainThick;         ///< Thickness of wall
  double mainXWidth;        ///< Cross width [full]

  double catchOut;           ///< length toward ring centre
  double catchThick;         ///< Thickness of wall
  double catchXWidth;        ///< Cross width [full]

  std::string wallMat;         ///< base material
    

 public:

  MazeGenerator();
  MazeGenerator(const MazeGenerator&);
  MazeGenerator& operator=(const MazeGenerator&);
  ~MazeGenerator();


  // L/T/W
  void setMain(const double,const double,const double);
  void setCatch(const double,const double,const double);

  void generateMaze(FuncDataBase&,const std::string&,
		      const double) const;

};

}

#endif
 
