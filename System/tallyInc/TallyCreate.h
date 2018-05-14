/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/TallyCreate.h
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
#ifndef tallySystem_TallyCreate_h
#define tallySystem_TallyCreate_h

class Simulation;
class SimMCNP;

namespace tallySystem
{
  class sswTally;
  
  void setComment(SimMCNP&,const int,const std::string&);
  void cutTallyEnergy(SimMCNP&,const double);
  void setTallyTime(SimMCNP&,const int,const std::string&);

  void addFullHeatBlock(SimMCNP&);             
  void addHeatBlock(SimMCNP&,const std::vector<int>&);             

  int getLastTallyNumber(const SimMCNP&,const int);

  sswTally* addSSWTally(SimMCNP&);
  
  void addF1Tally(SimMCNP&,const int,const int);  //
  void addF1Tally(SimMCNP&,const int,
		  const int,const std::vector<int>&);  

  void addF4Tally(SimMCNP&,const int,const std::string&,const std::vector<int>&);    

  void addF7Tally(SimMCNP&,const int,const std::vector<int>&);    

  void addF5Tally(SimMCNP&,const int);
  void addF5Tally(SimMCNP&,const int,const Geometry::Vec3D&,
		  const std::vector<Geometry::Vec3D>&,const double= 0.0);
  void setF5Position(SimMCNP&,const int,const Geometry::Vec3D&,
		     const Geometry::Vec3D&,const double,const double);
  void setF5Position(SimMCNP&,const int,const Geometry::Vec3D&);
  void setF5Angle(SimMCNP&,const int,const Geometry::Vec3D&,   
		  const double,const double);
  void modF5TallyCells(SimMCNP&,const int,const std::vector<int>&);
  void moveF5Tally(SimMCNP&,const int,const Geometry::Vec3D&);
  void widenF5Tally(SimMCNP&,const int,const int,const double);
  void slideF5Tally(SimMCNP&,const int,const int,const double);
  void shiftF5Tally(SimMCNP&,const int,const double);
  void divideF5Tally(SimMCNP&,const int,const int,const int);

  void addF6Tally(SimMCNP&,const int,const std::string&,
		  const std::vector<int>&);

  void writePlanes(const int,const int,const std::vector<int>&);

  int changeParticleType(SimMCNP&,const int,
			 const std::string&,const std::string&);

  int getFarPoint(const SimMCNP&,Geometry::Vec3D&);
  int setParticleType(SimMCNP&,const int,const std::string&);
  int setEnergy(SimMCNP&,const int,const std::string&);
  int setTime(SimMCNP&,const int,const std::string&);
  int setAngle(SimMCNP&,const int,const std::string&);
  int setFormat(SimMCNP&,const int,const std::string&);
  int setSDField(SimMCNP&,const int,const std::string&);
  int setSingle(SimMCNP&,const int);
  int setFormat(SimMCNP&,const int,const std::string&);
  
  void mergeTally(SimMCNP&,const int,const int);
  void deleteTallyType(SimMCNP&,const int);
  void deleteTally(SimMCNP&,const int);


  void addPointPD(SimMCNP&);
  void removeF5Window(SimMCNP&,const int);

  // WRONG PLACE
  void addXMLtally(Simulation&,const std::string&);

}  // namespace tallySystem 

#endif
