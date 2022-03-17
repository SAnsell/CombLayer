/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   LinacInc/LowBeamBox.h
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
#ifndef tdcSystem_LowBeamBox_h
#define tdcSystem_LowBeamBox_h

class Simulation;

namespace tdcSystem
{
/*!
  \class LowBeamBox
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief Plate inserted in object 
  
  Designed to be a quick plate to put an object into a model
  for fluxes/tallies etc
*/

class LowBeamBox :
    public attachSystem::ContainedComp,
    public attachSystem::FixedRotate,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
{
 private:
    
  double width;             ///< Full Width
  double length;            ///< Full Depth
  double fullCut;           ///< Full outer cut
  double innerCut;          ///< Inner cut thickness
  double backThick;         ///< back thickness
  double b4cThick;          ///< extra b4c layer thickness
  double backExtension;     ///< back extentions
  double wallThick;         ///< front thickness

  int innerMat;             ///< Material
  int backMat;              ///< Material for back wall
  int b4cMat;               ///< Material for back wall
  int mainMat;              ///< Material
  
  virtual void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  

 public:

  LowBeamBox(const std::string&);
  LowBeamBox(const std::string&,const std::string&);
  LowBeamBox(const LowBeamBox&);
  LowBeamBox& operator=(const LowBeamBox&);
  virtual ~LowBeamBox();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
