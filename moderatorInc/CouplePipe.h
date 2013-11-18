/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/CouplePipe.h
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
#ifndef moderatorSystem_CouplePipe_h
#define moderatorSystem_CouplePipe_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class CouplePipe
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief CouplePipe [insert object]
*/

class CouplePipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  
  ModelSupport::PipeLine GOuter; ///< Global outer
  ModelSupport::PipeLine HInner; ///< Inner Hydrogen pipe

  double Xoffset;              ///< Pipe offset [x]
  double Yoffset;              ///< Pipe offset [y]

  int outMat;                  ///< Outer material
  int outAlMat;                ///< Outer material
  int outVacMat;               ///< Outer material
  int innerAlMat;              ///< Outer material
  int innerMat;                ///< Outer material

  double outRadius;            ///< Outer Radius
  double outAlRadius;          ///< Outer Al-Radius
  double outVacRadius;         ///< Outer Vac-Thickness
  double innerAlRadius;        ///< Inner Al-Thickness
  double innerRadius;          ///< Inner Radius

  double fullLen;              ///< Outer Height

  // Hydrogen Pipe stuff:
  int hydMat;                ///< Hydrogen material
  int hydWallMat;            ///< Inner material
  double hydTemp;            ///< Moderator material
  
  double hXoffset;          ///< Pipe offset [x]
  double hYoffset;          ///< Pipe offset [y]
  double hLen;              ///< length of pipe
  double hRadius;           ///< Outer radius
  double hThick;            ///< Wall thickness
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&,const size_t);

  void insertPipes(Simulation&,const VacVessel&);
  void insertOuter(Simulation&,const VacVessel&);

 public:

  CouplePipe(const std::string&);
  CouplePipe(const CouplePipe&);
  CouplePipe& operator=(const CouplePipe&);
  ~CouplePipe();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const size_t,const VacVessel&);

};

}

#endif
 
