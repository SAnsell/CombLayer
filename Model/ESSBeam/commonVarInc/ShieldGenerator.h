/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essInc/ShieldGenerator.h
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
#ifndef setVariable_ShieldGenerator_h
#define setVariable_ShieldGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ShieldGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief ShieldGenerator for variables
*/

class ShieldGenerator
{
 private:

  /// storage for layers
  typedef std::map<size_t,double> MLTYPE;
  /// storage for layer material
  typedef std::map<size_t,std::string> MSTYPE;

  double leftAngle;             ///< Left wall angle 
  double rightAngle;            ///< Right wall angle
  double endThick;              ///< End wall thick
  double endRadius;              ///< End wall thick
  
  size_t nWall;                 ///< number of wall layers
  size_t nRoof;                 ///< number of roof layers
  size_t nFloor;                ///< number of floor layers  
  
  std::string defMat;                         ///< Primary default mat

  std::map<size_t,double> wallLen;            ///< wall lengths
  std::map<size_t,double> roofLen;            ///< roof lenghts
  std::map<size_t,double> floorLen;           ///< floor lengths

  std::map<size_t,std::string> wallMat;       ///< wall mat changes
  std::map<size_t,std::string> roofMat;       ///< roof mat changes
  std::map<size_t,std::string> floorMat;      ///< floor mat changes

  
  void setLayers(MLTYPE&,MSTYPE&,double&,size_t&,
		 const size_t,const double,
		 const std::vector<double>&,
		 const std::vector<std::string>&);
  
  void processLayers(FuncDataBase&,const std::string&) const;
  
 public:

  ShieldGenerator();
  ShieldGenerator(const ShieldGenerator&);
  ShieldGenerator& operator=(const ShieldGenerator&);
  ~ShieldGenerator();  

  void setAngle(const double,const double);
  void setEndWall(const double,const double);
    
  void clearLayers();
  void addFloor(const size_t,const double,const std::string&);
  void addRoof(const size_t,const double,const std::string&);
  void addWall(const size_t,const double,const std::string&);

  void addWallLen(const size_t,const double);
  void addRoofLen(const size_t,const double);
  void addFloorLen(const size_t,const double);

  void addWallMat(const size_t,const std::string&);
  void addRoofMat(const size_t,const std::string&);
  void addFloorMat(const size_t,const std::string&);

  void setRFLayers(const size_t,const size_t);

  void generateShield(FuncDataBase&,const std::string&,
		      const double,const double,const double,const double,
		      const size_t,const size_t)  const;

  void generateTriShield(FuncDataBase&,const std::string&,
			 const double,const double,const double,const double,
			 const size_t,const size_t)  const;
};

}

#endif
 
