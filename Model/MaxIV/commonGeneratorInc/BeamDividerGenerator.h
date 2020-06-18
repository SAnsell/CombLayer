/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/BeamDividerGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef setVariable_BeamDividerGenerator_h
#define setVariable_BeamDividerGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BeamDividerGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief BeamDividerGenerator for variables
*/

class BeamDividerGenerator 
{
 private:

  double boxLength;                ///< void length [total]
  double wallThick;                ///< void radius
  double mainWidth;                ///< box width on left [front]
  double exitWidth;                ///< box width on right from exitOrg
  double height;                   ///< box height

  double mainXStep;                ///< Main step
  double exitXStep;                ///< Exit step [from box end]
  double exitAngle;                ///< angle on right

  double mainLength;               ///< Main length 
  double mainRadius;               ///< main radius
  double mainThick;                ///< main wall thickness

  double exitLength;               ///< Exit length [from box end]
  double exitRadius;               ///< exit radius
  double exitThick;                ///< exit wall thickness

  double flangeARadius;            ///< Joining Flange radius 
  double flangeALength;            ///< Joining Flange length

  double flangeBRadius;            ///< Joining Flange radius 
  double flangeBLength;            ///< Joining Flange length

  double flangeERadius;            ///< Exit flange radius
  double flangeELength;            ///< Exit flange length
  
  std::string voidMat;             ///< void material
  std::string wallMat;             ///< wall material
  std::string flangeMat;           ///< flange material  
  
 public:

  template<typename CF>
  BeamDividerGenerator(const CF&);
  BeamDividerGenerator(const BeamDividerGenerator&);
  BeamDividerGenerator& operator=(const BeamDividerGenerator&);
  virtual ~BeamDividerGenerator();

  void setMainSize(const double,const double);
  
  template<typename T> void setAFlangeCF();
  template<typename T> void setBFlangeCF();
  template<typename T> void setEFlangeCF();
  
  virtual void generateDivider(FuncDataBase&,
			       const std::string&,
			       const double =0.0,          // angle
			       const bool =0,
			       int =0)  const;

};

}

#endif
 
