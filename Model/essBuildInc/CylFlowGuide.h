/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/CylFlowGuide.h
 *
 * Copyright (c) 2004-2017 by Konstatin Batkov/Stuart Ansell
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
#ifndef essSystem_CylFlowGuide_h
#define essSystem_CylFlowGuide_h

class Simulation;

namespace essSystem
{
/*!
  \class CylFlowGuide
  \author K. Batkov
  \version 1.1
  \date August 2015
  \brief Inner structure of flow guide (engineering details)
*/

class CylFlowGuide : public attachSystem::ContainedComp,
  public attachSystem::FixedComp,
  public attachSystem::CellMap
{
 private:
  
  const int insIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double radius;                  ///< Main radius [from built object]
  double wallThick;               ///< Wall thickness
  int wallMat;                    ///< Wall material
 
  double gapWidth;                ///< Gap to allow water to flow
  size_t nBaffles;                ///< Number of baffles

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&,attachSystem::FixedComp&,
		     const long int);
  void createLinks();

 public:

  CylFlowGuide(const std::string&);
  CylFlowGuide(const CylFlowGuide&);
  CylFlowGuide& operator=(const CylFlowGuide&);
  virtual CylFlowGuide* clone() const;
  virtual ~CylFlowGuide();

  void createAll(Simulation&,attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
