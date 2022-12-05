/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/BasicFlightLine.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_BasicFlightLine_h
#define constructSystem_BasicFlightLine_h

class Simulation;

namespace constructSystem
{

/*!
  \class BasicFlightLine
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief BasicFlightLine [insert object]
*/

class BasicFlightLine :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedGroup,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:
  
  double anglesXY[2];           ///< Rotation in the XY plane 
  double anglesZ[2];            ///< Rotation in the Z plane
  
  double height;                ///< Height of flight line
  double width;                 ///< Width of flight line

  int innerMat;                 ///< Inner material
  
  size_t nLayer;                ///< Number of layers
  std::vector<double> lThick;   ///< Linear Thickness 
  std::vector<int> lMat;        ///< Layer Material

  int tapFlag;                  ///< Taper type flag
  std::string attachRule;       ///< Attached rule
  
  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void removeObjects(Simulation&);

 public:

  BasicFlightLine(const std::string&);
  BasicFlightLine(const BasicFlightLine&);
  BasicFlightLine& operator=(const BasicFlightLine&);
  ~BasicFlightLine();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);


  
};

}

#endif
 
