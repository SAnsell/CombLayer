/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/TwinPipe.h
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
#ifndef xraySystem_TwinPipe_h
#define xraySystem_TwinPipe_h

class Simulation;

namespace xraySystem
{

/*!
  \class TwinPipe
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief TwinPipe unit [simplified round pipe]
*/

class TwinPipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedSpace,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double coJoinLength;          ///< Length of common pipe
  double coJoinRadius;          ///< Radius of common pipe
  double coJoinThick;           ///< Thickness of outer merge plate
  
  double pipeAXYAngle;          ///< Rotation of pipe A
  double pipeAZAngle;           ///< Rotation of pipe A about X
  double pipeARadius;           ///< void radius [inner] 
  double pipeALength;           ///< void length [total]
  double pipeAThick;            ///< pipe wall thickness

  double pipeBXYAngle;          ///< Rotation of pipe A
  double pipeBZAngle;           ///< Rotation of pipe A about X
  double pipeBRadius;           ///< void radius [inner] 
  double pipeBLength;           ///< void length [total]
  double pipeBThick;          ///< pipe wall thickness

  double flangeCJRadius;          ///< Initioal Flange radius [-ve for rect]
  double flangeCJLength;          ///< Initial Flange length

  double flangeARadius;          ///< PipeA Flange radius 
  double flangeALength;          ///< PipeA Flange length

  double flangeBRadius;          ///< PipeB Flange radius 
  double flangeBLength;          ///< PipeB Flange length

  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material
    
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

  void applyActiveFrontBack();
  
 public:

  TwinPipe(const std::string&);
  TwinPipe(const TwinPipe&);
  TwinPipe& operator=(const TwinPipe&);
  virtual ~TwinPipe();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
