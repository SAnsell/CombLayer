/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/DecouplePipe.h
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
#ifndef moderatorSystem_DecouplePipe_h
#define moderatorSystem_DecouplePipe_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class DecouplePipe
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief DecouplePipe [insert object]
*/

class DecouplePipe : public attachSystem::FixedComp
{
 private:
  
  const int pipeIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index
  int populated;                ///< 1:var,2:axis,4:cent,8:face,16:cell
  
  ModelSupport::PipeLine Outer;   ///< Outer 
  ModelSupport::PipeLine HeIn;    ///< In-going He
  ModelSupport::PipeLine HeOut;   ///< Out-Going He
  ModelSupport::PipeLine CH4In;   ///< In-going CH4
  ModelSupport::PipeLine CH4Out;  ///< Outgoing CH4

  double Xoffset;              ///< Pipe offset [x]
  double Yoffset;              ///< Pipe offset [y]

  double HeInXStep;            ///< He In pipe [x]
  double HeInYStep;            ///< He In pipe [y]
  /// Track points
  std::vector<Geometry::Vec3D> HeTrack;  

  double HeOutXStep;           ///< He Out pipe [x]
  double HeOutYStep;           ///< He Out pipe [y]

  int outMat;                  ///< Outer material
  int outAlMat;                ///< Outer material
  int outVacMat;               ///< Outer material
  int innerAlMat;              ///< Outer material
  int innerMat;                ///< Outer material
  int heMat;                   ///< Helium material
  int heAlMat;                 ///< Helium material

  double outRadius;            ///< Outer Radius
  double outAlRadius;          ///< Outer Al-Radius
  double outVacRadius;         ///< Outer Vac-Thickness
  double innerAlRadius;        ///< Inner Al-Thickness
  double innerRadius;          ///< Inner Radius

  double heRadius;             ///< He Radius
  double heAlRadius;           ///< He Radius of Al

  double fullLen;              ///< Outer Height
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void insertPipes(Simulation&);
  void insertOuter(Simulation&,const VacVessel&);
  void insertHePipe(Simulation&,const VacVessel&);

 public:

  DecouplePipe(const std::string&);
  DecouplePipe(const DecouplePipe&);
  DecouplePipe& operator=(const DecouplePipe&);
  ~DecouplePipe();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const VacVessel&,const int);

};

}

#endif
 
