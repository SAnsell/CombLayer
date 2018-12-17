/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/GratingUnitGenerator.h
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
#ifndef setVariable_GratingUnitGenerator_h
#define setVariable_GratingUnitGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class GratingUnitGenerator
  \version 1.0
  \author S. Ansell
  \date July 2018
  \brief GratingUnitGenerator for variables
*/

class GratingUnitGenerator
{
 private:
  
    
  double gWidth;            ///< Radius of from centre
  double gThick;            ///< Radius of detector
  double gLength;           ///< Outer wall thickness

  double mainGap;           ///< Void gap between bars
  double mainBarXLen;       ///< X length of bars (to side support)
  double mainBarDepth;      ///< Depth Z direction
  double mainBarYWidth;     ///< Bar extent in beam direction

  double slidePlateZGap;       ///< lift from the mirror surface
  double slidePlateThick;      ///< slide bar
  double slidePlateWidth;      ///< slide bar extra width 
  double slidePlateLength;     ///< slide bar extra length

  double sideThick;
  double sideHeight;

  double endWidth;
  double endHeight;
  double endThick;
  
  std::string xstalMat;            ///< Xtal metal
  std::string mainMat;            ///< Main metal
  std::string slideMat;           ///< slide material


 public:

  GratingUnitGenerator();
  GratingUnitGenerator(const GratingUnitGenerator&);
  GratingUnitGenerator& operator=(const GratingUnitGenerator&);
  ~GratingUnitGenerator();


  void generateGrating(FuncDataBase&,const std::string&,
		       const double,const double) const;

};

}

#endif
 
