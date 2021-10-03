/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachCompInc/FixedRotateGroup.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef attachSystem_FixedRotateGroup_h
#define attachSystem_FixedRotateGroup_h

class FuncDataBase;

namespace attachSystem
{

/*!
  \struct offset 
  \version 1.0
  \author S. Ansell
  \date October 2015
  \brief Holding system to control offsets
*/
  
struct rotate
{
  double preXAngle;      ///< pre-shift x Angle
  double preYAngle;      ///< pre-shift y Angle
  double preZAngle;      ///< pre-shift z Angle

  double xStep;          ///< x step
  double yStep;          ///< y step
  double zStep;          ///< z step
  
  double xAngle;         ///< x Angle
  double yAngle;         ///< y Angle
  double zAngle;         ///< z Angle

  
  rotate() :
     preXAngle(0.0),preYAngle(0.0),preZAngle(0.0),
       xStep(0.0),yStep(0.0),zStep(0.0),
       xAngle(0.0),yAngle(0.0),zAngle(0.0) {}
       
  
    void copy(double&,double&,double&,
	    double&,double&,double&,
	    double&,double&,double&) const;
};
  
/*!
  \class FixedRotateGroup
  \version 1.0
  \author S. Ansell
  \date  August 2020
  \brief FixedComp derivative which handles x-z step and x/y/z angle rotation
  for a set of FixedComp groups
*/

class FixedRotateGroup  : public FixedGroup
{
 protected:

  double preXAngle;      ///< pre-shift x Angle
  double preYAngle;      ///< pre-shift y Angle
  double preZAngle;      ///< pre-shift z Angle

  double xStep;          ///< x step
  double yStep;          ///< y step
  double zStep;          ///< z step

  double xAngle;         ///< x Angle
  double yAngle;         ///< y Angle
  double zAngle;         ///< z Angle


  /// Mapping of group and local offset
  std::map<std::string,rotate> GOffset;
  
  static void populateRotate(const FuncDataBase&,const std::string&,
			     rotate&);
  
  static void populateRotate
    (const std::map<std::string,std::vector<std::string>>&,
     const std::string&,rotate&);
  
 public:

  FixedRotateGroup(const std::string&,const std::string&,
		   const size_t);
  FixedRotateGroup(const std::string&,const std::string&,
		   const size_t,const std::string&,const size_t);
  FixedRotateGroup(const std::string&,const std::string&,
		   const size_t,const std::string&,const size_t,
		   const std::string&,const size_t);
  FixedRotateGroup(const std::string&,const std::string&,const size_t,
		   const std::string&,const size_t,
		   const std::string&,const size_t,
		   const std::string&,const size_t);
  FixedRotateGroup(const FixedRotateGroup&);
  FixedRotateGroup& operator=(const FixedRotateGroup&);
  virtual ~FixedRotateGroup() {}     ///< Destructor

  virtual void populate(const FuncDataBase&);
  virtual void populate(const std::map<std::string,
			std::vector<std::string>>&);
  virtual void populate(const std::string&,const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,const long int);

  void applyOffset();
};

}

#endif
 
