/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/ControlElement.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef delftSystem_ControlElement_h
#define delftSystem_ControlElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class ControlElement
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Modifies a fuel element to take a control rod
*/

class ControlElement : public FuelElement,
    public attachSystem::ContainedGroup
{
 private:
  
  const std::string cntlKey;          ///< Control keyname
  const int controlIndex;             ///< Local control number 

  size_t cStartIndex;           ///< Element for control portion
  size_t cEndIndex;             ///< Element for control portion

  double lift;                  ///< Rise of the control rod
  
  double voidRadius;            ///< Radius of voids
  double voidOffset;            ///< offset from centre
  double absThick;              ///< B4C radius/thickness [2xradius]
  double caseThick;             ///< Case thickness
  double waterThick;            ///< Case thickness
  double plateThick;            ///< Al thickness
  double innerLength;           ///< Inner control rod length

  double outerLength;           ///< Outer Al holder rod length [full]
  double outerCapLen;           ///< Cap a top of Al track


  double endCap;                ///< Rod end cap

  int absMat;                   ///< control material
  int cladMat;                  ///< Cladding material

  void populate(const FuncDataBase&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  ControlElement(const size_t,const size_t,
		 const std::string&,const std::string&);
  ControlElement(const ControlElement&);
  ControlElement& operator=(const ControlElement&);
  virtual ~ControlElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const FixedComp&,
			 const Geometry::Vec3D&,const FuelLoad&);

};

}  

#endif
