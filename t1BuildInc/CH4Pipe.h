/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/CH4Pipe.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef moderatorSystem_CH4Pipe_h
#define moderatorSystem_CH4Pipe_h

class Simulation;

namespace ts1System
{

/*!
  \class CH4Pipe
  \version 1.0
  \author G Skoro
  \date February 2012
  \brief CH4Pipe [insert object]
*/

class CH4Pipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  ModelSupport::PipeLine Central; ///< Global outer

  double Xoffset;              ///< Pipe offset [x]
  double Yoffset;              ///< Pipe offset [y]

  int ch4Mat;                  ///< Hydrogen material
  int alMat;                  ///< Al material

  double ch4Temp;              ///< CH4 Temperature

  double ch4InnerRadius;       ///< Inner radius [all centralized]
  double alInnerRadius;       ///< Al on inner
  double ch4OuterRadius;
  double alOuterRadius;       ///< Al on outer
  double vacRadius;           ///< Vac Radius
  double alTerRadius;           ///< Al on teriary
  double clearRadius;           ///< Clearance

  double fullLen;              ///< Outer Height
  double depthLen;             ///< Inner depth into moderator
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void insertPipes(Simulation&);
  void insertOuter(Simulation&,const attachSystem::FixedComp&,const size_t);

 public:

  CH4Pipe(const std::string&);
  CH4Pipe(const CH4Pipe&);
  CH4Pipe& operator=(const CH4Pipe&);
  ~CH4Pipe();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);

};

}

#endif
 
