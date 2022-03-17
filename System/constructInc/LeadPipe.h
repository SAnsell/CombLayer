/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/LeadPipe.h
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
#ifndef constructSystem_LeadPipe_h
#define constructSystem_LeadPipe_h

class Simulation;

namespace constructSystem
{

/*!
  \class LeadPipe
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief LeadPipe unit [simplified round pipe]
*/

class LeadPipe :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedGroup,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::ExternalCut
    
{
 private:

  bool outerVoid;              ///< Make an outer void
  
  double radius;               ///< void radius [inner]
  double length;               ///< void length [total]
  double thick;            ///< pipe thickness

  double claddingThick;        ///< cladding thickness
  double claddingStep;         ///< distance from front/back to cladding start    
  double flangeARadius;        ///< Joining FlangeA radius 
  double flangeBRadius;        ///< Joining FlangeB radius 

  double flangeALength;        ///< Joining Flange length
  double flangeBLength;        ///< Joining Flange length

  int voidMat;                 ///< Void material
  int pipeMat;                 ///< Pipe material
  int claddingMat;             ///< Pipe cladding material
  int flangeMat;               ///< Flange material
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  LeadPipe(const std::string&);
  LeadPipe(const LeadPipe&) =default;
  LeadPipe& operator=(const LeadPipe&) =default;
  virtual ~LeadPipe();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const FixedComp&,
			 const long int);
};

}

#endif
 
