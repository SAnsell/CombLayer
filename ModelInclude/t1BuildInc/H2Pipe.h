/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/H2Pipe.h
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
#ifndef ts1System_H2Pipe_h
#define ts1System_H2Pipe_h

class Simulation;

namespace ts1System
{

/*!
  \class H2Pipe
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief H2Pipe [insert object]
*/

class H2Pipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  ModelSupport::PipeLine Central; ///< Global outer

  double Xoffset;              ///< Pipe offset [x]
  double Yoffset;              ///< Pipe offset [y]

  int h2Mat;                  ///< Hydrogen material
  int alMat;                  ///< Al material
  int steelMat;               ///< SS material (inner pipe only)
  double h2Temp;              ///< H2 Temperature

  double h2InnerRadius;       ///< Inner radius [all centralized]
  double alInnerRadius;       ///< Al on inner
  double h2OuterRadius;
  double alOuterRadius;       ///< Al on outer
  double vacRadius;           ///< Vac Radius
  double midRadius;           ///< Mid Radius
  double terRadius;           ///< Mid Radius
  double alTerRadius;         ///< Al on teriary
  double clearRadius;         ///< Clearance
  double steelRadius;         ///< Steel layer in reflector

  double fullLen;              ///< Outer Height
  double depthLen;             ///< Inner depth into moderator
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void insertPipes(Simulation&);
  void insertOuter(Simulation&,const attachSystem::FixedComp&,const size_t);

 public:

  H2Pipe(const std::string&);
  H2Pipe(const H2Pipe&);
  H2Pipe& operator=(const H2Pipe&);
  ~H2Pipe();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);

};

}

#endif
 
