/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/TallyCreate.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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


namespace tallySystem
{
  class sswTally;
  
  void setComment(Simulation&,const int,const std::string&);
  void cutTallyEnergy(Simulation&,const double);
  void setTallyTime(Simulation&,const int,const std::string&);

  void addFullHeatBlock(Simulation&);             
  void addHeatBlock(Simulation&,const std::vector<int>&);             

  int getLastTallyNumber(const Simulation&,const int);

  sswTally* addSSWTally(Simulation&);
  
  void addF1Tally(Simulation&,const int,const int);  // Add basic F1 tally
  void addF1Tally(Simulation&,const int,
		  const int,const std::vector<int>&);  

  void addF4Tally(Simulation&,const int,const std::string&,const std::vector<int>&);    

  void addSimpleF5Tally(Simulation&,const int,const Geometry::Vec3D&,
			const std::string&,const std::string&,const std::string&); 
  void addF7Tally(Simulation&,const int,const std::vector<int>&);    

  void addF5Tally(Simulation&,const int);
  void addF5Tally(Simulation&,const int,const Geometry::Vec3D&,
		  const std::vector<Geometry::Vec3D>&,const double= 0.0);
  void setF5Position(Simulation&,const int,const Geometry::Vec3D&,
		     const Geometry::Vec3D&,const double,const double);
  void setF5Position(Simulation&,const int,const Geometry::Vec3D&);
  void setF5Angle(Simulation&,const int,const Geometry::Vec3D&,   
		  const double,const double);
  void modF5TallyCells(Simulation&,const int,const std::vector<int>&);
  void moveF5Tally(Simulation&,const int,const Geometry::Vec3D&);
  void widenF5Tally(Simulation&,const int,const int,const double);
  void slideF5Tally(Simulation&,const int,const int,const double);
  void shiftF5Tally(Simulation&,const int,const double);
  void divideF5Tally(Simulation&,const int,const int,const int);

  void addF6Tally(Simulation&,const int,const std::string&,
		  const std::vector<int>&);

  void writePlanes(const int,const int,const std::vector<int>&);

  int changeParticleType(Simulation&,const int,
			 const std::string&,const std::string&);

  int getFarPoint(const Simulation&,Geometry::Vec3D&);
  int setParticleType(Simulation&,const int,const std::string&);
  int setEnergy(Simulation&,const int,const std::string&);
  int setTime(Simulation&,const int,const std::string&);
  int setAngle(Simulation&,const int,const std::string&);
  int setFormat(Simulation&,const int,const std::string&);
  int setSDField(Simulation&,const int,const std::string&);
  int setSingle(Simulation&,const int);
  int setFormat(Simulation&,const int,const std::string&);
  
  void mergeTally(Simulation&,const int,const int);
  void deleteTallyType(Simulation&,const int);
  void deleteTally(Simulation&,const int);


  void addPointPD(Simulation&);
  void removeF5Window(Simulation&,const int);

  void addXMLtally(Simulation&,const std::string&);

}  // namespace tallySystem 

#endif
