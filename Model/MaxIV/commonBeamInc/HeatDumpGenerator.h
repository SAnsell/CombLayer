/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/HeatDumpGenerator.h
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
#ifndef setVariable_HeatDumpGenerator_h
#define setVariable_HeatDumpGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class HeatDumpGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief HeatDumpGenerator for variables
*/

class HeatDumpGenerator
{
 private:

  double radius;           ///< Radius 
  double height;           ///< height total 
  double width;            ///< width accross beam
  double thick;            ///< Thickness in normal direction to reflection  
  double lift;             ///< Amount to lift [when raized]
  
  double cutHeight;        ///< Z from base [centre on beam axis] 
  double cutAngle;         ///< Angle of cut
  double cutDepth;         ///< cut distance in Y

  double topInnerRadius;          ///<  Flange inner radius radius 
  double topFlangeRadius;          ///< Joining Flange radius 
  double topFlangeLength;          ///< Joining Flange length

  double bellowLength;             ///< Bellow length
  double bellowThick;              ///< Bellow thick
  
  double outRadius;                ///< Out connect radius
  double outLength;                ///< Out connect length
  
  std::string mat;                 ///< Base material
  std::string flangeMat;           ///<  flange material
  std::string bellowMat;           ///<  bellow material 


 public:

  HeatDumpGenerator();
  HeatDumpGenerator(const HeatDumpGenerator&);
  HeatDumpGenerator& operator=(const HeatDumpGenerator&);
  virtual ~HeatDumpGenerator();

  template<typename CF> void setCF();
  void setMat(const std::string&,const double);
  
  void generateHD(FuncDataBase&,const std::string&,
		  const bool) const;

};

}

#endif
 
