/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/LBeamStop.h
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
#ifndef tdcSystem_LBeamStop_h
#define tdcSystem_LBeamStop_h

class Simulation;

namespace tdcSystem
{

/*!
  \class LBeamStop
  \version 1.0
  \author Stuart Ansell
  \date November 2020
  \brief Simple BeamStop for TDC
*/

class LBeamStop :
  public attachSystem::ContainedComp,
  public attachSystem::FixedRotate,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::ExternalCut
{
 private:

  double length;                ///< Total length including void

  double innerVoidLen;         ///< inner void lenth
  double innerLength;	       ///< inner length of material
  double innerRadius;	       ///< inner radius

  double midVoidLen;            ///< mid void lenth
  double midLength;             ///< mid length of material
  double midRadius;             ///< mid radius
  size_t midNLayers;            ///< Number of layers in the mid region

  double outerRadius;           ///< Outer radius
  size_t outerNLayers;             ///< Number of layers in the outer region

  int voidMat;                  ///< Void material
  int innerMat;                 ///< Inner material
  int midMat;                   ///< Mid material
  int outerMat;                 ///< Outer material

  void layerProcess(Simulation&,const std::string&,
                    const int,const int,const size_t);

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  LBeamStop(const std::string&);
  LBeamStop(const LBeamStop&);
  LBeamStop& operator=(const LBeamStop&);
  virtual ~LBeamStop();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
