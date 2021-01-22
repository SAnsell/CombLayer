/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1CommonInc/DipoleExtract.h
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
#ifndef xraySystem_DipoleExtract_h
#define xraySystem_DipoleExtract_h

class Simulation;


namespace xraySystem
{
/*!
  \class DipoleExtract
  \version 1.0
  \author S. Ansell
  \date October 2020

  \brief DipoleExtract for Max-IV 

  This is the extacttion section in the dipole chamber of R1 
  between dipole magnets
*/

class DipoleExtract :
  public attachSystem::FixedRotate,
  public attachSystem::ContainedComp,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
  
  double length;                ///< frame length
  double width;                 ///< width of inner frame
  double height;                ///< Inner height [straight]
  double wideHeight;            ///< Height at end of triangle
  double wideWidth;             ///< Width at which the triangle stops
  
  double exitLength;             ///< Cut in exit channel

  double wallThick;             ///< Wall thickness
  double edgeThick;             ///< Edge Wall thickness
  
  int voidMat;                  ///< void material
  int wallMat;                  ///< wall material
  int outerMat;                 ///< inner material
  
  void populate(const FuncDataBase&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DipoleExtract(const std::string&);
  DipoleExtract(const DipoleExtract&);
  DipoleExtract& operator=(const DipoleExtract&);
  virtual ~DipoleExtract();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
