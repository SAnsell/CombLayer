/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/IrradElement.h
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
#ifndef delftSystem_IrradElement_h
#define delftSystem_IrradElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class IrradElement
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Creates an irradiation element
*/

class IrradElement  : public RElement
{
 private:

  double Width;           ///< Width of outer
  double Depth;           ///< Depth of outer

  double beLen;              ///< Total depth of the cell
  double topPlug;            ///< End cap thickness
  double topLocator;         ///< End cap thickness
  double endStop;            ///< End cap thickness
  double plugThick;          ///< Inner plug thickness
  double plugZOffset;        ///< Inner plug offset
  double locThick;           ///< Locator thickness (wall)

  double sampleRadius;    ///< Inner radius of test unit
  double sampleXOff;      ///< X offset
  double sampleZOff;      ///< Z offset
  double vacRadius;       ///< Initial vac 
  double caseRadius;      ///< Case Radius [water cooled]
  double coreRadius;      ///< Core Raidus
  double outerRadius;     ///< outer radius
  double waterRadius;     ///< water radius [outer]

  int beMat;              ///< Default [be material]
  int pipeMat;            ///< Al typically
  int sampleMat;          ///< sample [whaterever]
  int waterMat;           ///< Water coolant 

  void populate(const Simulation&);
  void createUnitVector(const FixedComp&,const Geometry::Vec3D&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  IrradElement(const size_t,const size_t,const std::string&);
  IrradElement(const IrradElement&);
  IrradElement& operator=(const IrradElement&);
  virtual ~IrradElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const Geometry::Vec3D&,const FuelLoad&);

};

}  

#endif
