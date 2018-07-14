/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balderInc/LeadBox.h
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
#ifndef xraySystem_LeadBox_h
#define xraySystem_LeadBox_h

class Simulation;

namespace xraySystem
{
  
/*!
  \class LeadBox
  \version 1.0
  \author S. Ansell
  \date July 2018
  \brief LeadBox unit  
  
  This provides shielding for bellow unit in lead etc
*/

class LeadBox :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedGroup,
  public attachSystem::CellMap,
  public attachSystem::ExternalCut
{
 private:


  
  double height;            ///< void height [top only]
  double depth;             ///< void depth [low only]
  double width;             ///< void width [total]
  double length;            ///< void length [total]

  double wallThick;
  double portGap;          ///< Clearance to port
  
  int voidMat;                ///< void material
  int wallMat;                  ///< Fe material layer
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  LeadBox(const std::string&);
  LeadBox(const LeadBox&);
  LeadBox& operator=(const LeadBox&);
  virtual ~LeadBox();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
