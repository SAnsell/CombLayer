/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/BWaterPipe.h
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
#ifndef bibSystem_WaterPipe_h
#define bibSystem_WaterPipe_h

class Simulation;

namespace bibSystem
{

/*!
  \class BWaterPiper
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief H2Pipe [insert object]
*/

class BWaterPipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  ModelSupport::PipeLine Central; ///< Global outer

  double Xoffset;              ///< Pipe offset [x]
  double Yoffset;              ///< Pipe offset [y]

  int waterMat;                  ///< Hydrogen material
  int alMat;                  ///< Al material

  double watRadius;       ///< Inner radius [all centralized]
  double wallRadius;       ///< Inner radius [all centralized]
  double clearRadius;       ///< Inner radius [all centralized]

  double fullLen;              ///< Outer Height
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void insertPipe(Simulation&,const attachSystem::FixedComp&,
		  const long int);

 public:

  BWaterPipe(const std::string&);
  BWaterPipe(const BWaterPipe&);
  BWaterPipe& operator=(const BWaterPipe&);
  ~BWaterPipe();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
