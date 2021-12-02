/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1BuildInc/WaterPipe.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef ts1System_WaterPipe_h
#define ts1System_WaterPipe_h

class Simulation;

namespace ts1System
{

/*!
  \class WaterPipe
  \version 1.0
  \author G. Skoro
  \date February 2013
  \brief WaterPipe [insert object]
*/

class WaterPipe :
    public attachSystem::FixedComp
{
 private:
  
  ModelSupport::PipeLine Inlet;   ///< Inlet water pipe
  ModelSupport::PipeLine Outlet;  ///< Outlet water pipe

  double iXStep;              ///< Pipe offset [x]
  double iYStep;              ///< Pipe offset [y]

  double oXStep;              ///< Pipe offset [x]
  double oYStep;              ///< Pipe offset [y]

  double watRadius;            ///< Water Radius
  double outRadius;            ///< Outer Radius
  double voidRadius;           ///< Void Radius
  double steelRadius;          ///< Reflector steel radius

  int outMat;                 ///< Outer material
  int watMat;                 ///< Outer material
  int steelMat;                 ///< Outer material

  double ifullLen;              ///< Outer Height
  double ofullLen;              ///< Outer Height
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void insertPipes(Simulation&);

 public:

  WaterPipe(const std::string&);
  WaterPipe(const WaterPipe&);
  WaterPipe& operator=(const WaterPipe&);
  ~WaterPipe();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
