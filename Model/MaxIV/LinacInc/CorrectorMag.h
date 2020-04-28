/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/CorrectorMag.h
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
#ifndef tdcSystem_CorrectorMag_h
#define tdcSystem_CorrectorMag_h

class Simulation;


namespace tdcSystem
{
/*!
  \class CorrectorMag
  \version 1.0
  \author S. Ansell
  \date April 2020

  \brief CorrectorMag for Max-IV

  This constructs a magnet in the horizontal direction
*/

class CorrectorMag :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap,
    public attachSystem::SurfMap
{
 private:

  const std::string baseName;   ///< Base key
  
  double magOffset;             ///< Magnet offset from pipe centre
  double magHeight;             ///< Magnet thickness
  double magWidth;              ///< Magnet thickness
  double magLength;             ///< Magnet full length
  double magCorner;             ///< Magnet corner radius
  double magInnerWidth;         ///< Magnet inner width
  double magInnerLength;        ///< Magnet inner length

  double pipeClampYStep;        ///< Pipe clamp out step
  double pipeClampZStep;        ///< Pipe clamp up step
  double pipeClampThick;        ///< Pipe clamp thickness     
  double pipeClampWidth;        ///< Pipe clamp width
  double pipeClampHeight;       ///< Pipe clamp height
  
  double frameHeight;            ///< hegiht of plate

  int voidMat;                     ///< coil material
  int coilMat;                     ///< coil material
  int clampMat;                    ///< clamp material
  int frameMat;                    ///< Iron material

  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks(const bool);

 public:

  CorrectorMag(const std::string&);
  CorrectorMag(const std::string&,const std::string&);
  CorrectorMag(const CorrectorMag&);
  CorrectorMag& operator=(const CorrectorMag&);
  virtual ~CorrectorMag();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
