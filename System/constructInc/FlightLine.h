/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/FlightLine.h
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

class FlightLine : public attachSystem::ContainedGroup,
    public attachSystem::FixedOffset
{
 private:
  
  const int flightIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double anglesXY[2];           ///< Rotation in the XY plane 
  double anglesZ[2];            ///< Rotation in the Z plane
  
  double height;                ///< Height of flight line
  double width;                 ///< Width of flight line

  long int plateIndex;          ///< Index of the side [+1] with sign

  int innerMat;                 ///< Inner material
  
  size_t nLayer;                ///< Number of layers
  std::vector<double> lThick;   ///< Linear Thickness 
  std::vector<int> lMat;        ///< Layer Material

  bool capActive;               ///< Using front/back cap
  std::vector<int> capLayer;    ///< End cap layers
  std::vector<HeadRule> capRule;   ///< Rule for each cap

  std::string attachRule;       ///< Attached rule
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const long int);
  //  void createUnitVector(const Geometry::Vec3D&,const Geometry::Vec3D&,
  //			const Geometry::Vec3D&);
  void createRotatedUnitVector(const attachSystem::FixedComp&,const long int,
			       const long int);

  void createSurfaces();
  void createCapSurfaces(const attachSystem::FixedComp&,const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int);
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,
		     const attachSystem::ContainedComp&);

  void removeObjects(Simulation&);
  std::string getRotatedDivider(const attachSystem::FixedComp&,
				const long int);

 public:

  FlightLine(const std::string&);
  FlightLine(const FlightLine&);
  FlightLine& operator=(const FlightLine&);
  ~FlightLine();

  void getInnerVec(std::vector<int>&) const;



  
  void reBoundary(Simulation&,const long int,
		  const attachSystem::FixedComp&);

  void processIntersectMajor(Simulation&,
			     const attachSystem::ContainedGroup&,
			     const std::string&,const std::string&);
  void processIntersectMinor(Simulation&,
			     const attachSystem::ContainedGroup&,
			     const std::string&,const std::string&);
  


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  void createAll(Simulation&,const long int,const long int,
		 const attachSystem::FixedComp&);
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::ContainedComp&,const long int =0);

};

}

#endif
 
