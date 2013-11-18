/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/MerlinPipe.h
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
#ifndef moderatorSystem_MerlinPipe_h
#define moderatorSystem_MerlinPipe_h

class Simulation;

namespace ts1System
{

/*!
  \class MerlinPipe
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief MerlinPipe [insert object]
*/

class MerlinPipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  ModelSupport::PipeLine Central; ///< Global outer
  ModelSupport::PipeLine WatIn;   ///< Inner Hydrogen pipe
  ModelSupport::PipeLine WatOut;  ///< Inner Hydrogen pipe

  double Xoffset;              ///< Pipe offset [x]
  double Yoffset;              ///< Pipe offset [y]

  int outMat;                 ///< Outer material
  int outVacMat;              ///< Outer material
  int midMat;                 ///< Outer material
  int midVacMat;              ///< Outer material
  int watSkinMat;             ///< Outer material
  int watMat;                 ///< Outer material

  double outRadius;            ///< Outer Radius
  double outVacRadius;         ///< Outer Al-Radius
  double midRadius;            ///< Outer Vac-Thickness
  double midVacRadius;         ///< Outer Vac-Thickness
  double watSkinRadius;        ///< Inner Al-Thickness
  double watRadius;            ///< Inner Radius

  double fullLen;              ///< Outer Height

  
  double wInXoffset;          ///< Pipe offset [x]
  double wInYoffset;          ///< Pipe offset [y]
  double wOutXoffset;          ///< Pipe offset [x]
  double wOutYoffset;          ///< Pipe offset [y]
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void insertPipes(Simulation&);
  void insertOuter(Simulation&,const attachSystem::FixedComp&,const size_t);

 public:

  MerlinPipe(const std::string&);
  MerlinPipe(const MerlinPipe&);
  MerlinPipe& operator=(const MerlinPipe&);
  ~MerlinPipe();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t);

};

}

#endif
 
