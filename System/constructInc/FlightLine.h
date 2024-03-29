/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/FlightLine.h
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
#ifndef moderatorSystem_FlightLine_h
#define moderatorSystem_FlightLine_h

class Simulation;


/*!
  \namespace moderatorSystem
  \version 1.0
  \author S. Ansell
  \date April 2013
  \brief Basic moderators [mainly TS2]
*/

namespace moderatorSystem
{

/*!
  \class FlightLine
  \version 1.0
  \author S. Ansell
  \date April 2011
  \brief FlightLine [insert object]
*/

class FlightLine :
    public attachSystem::ContainedGroup,
    public attachSystem::FixedRotate,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:
  
  double anglesXY[2];           ///< Rotation in the XY plane 
  double anglesZ[2];            ///< Rotation in the Z plane
  
  double height;                ///< Height of flight line
  double width;                 ///< Width of flight line

  long int plateIndex;          ///< Index of the side [+1] with sign

  int innerMat;                 ///< Inner material
  
  size_t nLayer;                ///< Number of layers
  std::vector<double> lThick;   ///< Linear Thickness 
  std::vector<int> lMat;        ///< Layer Material

  bool capActive;                  ///< Using front/back cap
  std::vector<int> capLayer;       ///< End cap layers
  std::vector<HeadRule> capRule;   ///< Rule for each cap

  HeadRule attachRuleHR;           ///< Attached rule
  
  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createCapSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);

  HeadRule getRotatedDivider(const attachSystem::FixedComp&,
			     const long int);

  
 public:

  FlightLine(const std::string&);
  FlightLine(const FlightLine&);
  FlightLine& operator=(const FlightLine&);
  ~FlightLine() override;

  void getInnerVec(std::vector<int>&) const;



  

  void processIntersectMajor(Simulation&,
			     const attachSystem::ContainedGroup&,
			     const std::string&,const std::string&);
  void processIntersectMinor(Simulation&,
			     const attachSystem::ContainedGroup&,
			     const std::string&,const std::string&);
  


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override;
 
};

}

#endif
 
