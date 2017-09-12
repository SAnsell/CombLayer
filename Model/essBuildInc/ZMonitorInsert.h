/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/ZMonitorInsert.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef essSystem_ZMonitorInsert_h
#define essSystem_ZMonitorInsert_h

class Simulation;

namespace essSystem
{

/*!
  \class ZMonitorInsert
  \version 1.0
  \author S. Ansell
  \date September 2017
  \brief Monitor plug above the target
*/

class ZMonitorInsert :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:
  
  const int monIndex;         ///< Index of surface offset
  int cellIndex;              ///< Cell index

  size_t nSegments;             ///< number of segments

  std::vector<double> innerRadii;    ///< Inner radii
  std::vector<double> outerRadii;    ///< Outer radii
  std::vector<double> height;        ///< Height of each segment
  std::vector<double> arcAngle;      ///< Angle of arc

  std::vector<int> mat;              ///< Materials
  

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  ZMonitorInsert(const std::string&);
  ZMonitorInsert(const ZMonitorInsert&);
  ZMonitorInsert& operator=(const ZMonitorInsert&);
  virtual ~ZMonitorInsert();
    
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 

