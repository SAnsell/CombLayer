/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/ShutterUnitGenerator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_ShutterUnitGenerator_h
#define setVariable_ShutterUnitGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ShutterUnitGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief ShutterUnitGenerator for variables
*/

class ShutterUnitGenerator
{
 private:

  double height;           ///< height total 
  double width;            ///< width accross beam
  double thick;            ///< Thickness in normal direction to reflection  
  double baseLift;         ///< Amount to lift [before raise]
  double lift;             ///< Amount to lift [when raised]
  double liftScrewRadius;  ///< Radius of lifting thread
  double threadLength;     ///< Total length of thread
    
  double topInnerRadius;          ///<  Flange inner radius radius 
  double topFlangeRadius;          ///< Joining Flange radius 
  double topFlangeLength;          ///< Joining Flange length

  double bellowLength;             ///< Bellow length
  double bellowThick;              ///< Bellow thick
  
  double outRadius;                ///< Out connect radius
  double outLength;                ///< Out connect length
  
  std::string mat;                 ///< Base material
  std::string threadMat;           ///< Thread material
  std::string flangeMat;           ///<  flange material
  std::string bellowMat;           ///<  bellow material 

 public:

  ShutterUnitGenerator();
  ShutterUnitGenerator(const ShutterUnitGenerator&);
  ShutterUnitGenerator& operator=(const ShutterUnitGenerator&);
  virtual ~ShutterUnitGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setOutCF();
  template<typename CF> void setTopCF();

  void setLift(const double,const double);
  void setMat(const std::string&,const double);
  
  void generateShutter(FuncDataBase&,const std::string&,
		  const bool) const;

};

}

#endif
 
