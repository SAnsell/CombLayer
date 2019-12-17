/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/WallLeadGenerator.h
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
#ifndef setVariable_WallLeadGenerator_h
#define setVariable_WallLeadGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class WallLeadGenerator
  \version 1.0
  \author S. Ansell
  \date January 2019
  \brief WallLeadGenerator for variables
*/

class WallLeadGenerator
{
 private:

  double xStep;               ///< X offset
  
  double frontLength;         ///< Front length
  double backLength;          ///< Back length

  double frontWidth;          ///< Front width of hole
  double frontHeight;         ///< Front height of hole

  double backWidth;           ///< Front width of hole
  double backHeight;          ///< Front height of hole

  double steelOutWidth;       ///< Thickness of steel 
  double steelRingWidth;      ///< Thickness of steel
  double steelHeight;         ///< Height of steel [up]
  double steelDepth;          ///< Height of steel [down]
  double steelThick;          ///< Depth of steel
  double steelXCut;           ///< Central hole size
  double steelZCut;           ///< Central hole size 

  double extraLeadOutWidth;   ///< Width of lead
  double extraLeadRingWidth;  ///< Ring Width of lead
  double extraLeadHeight;     ///< Height of lead
  double extraLeadDepth;      ///< Depth of lead
  double extraLeadXCut;       ///< Cut out
  
  std::string voidMat;                 ///< void material
  std::string midMat;                  ///< mid material
  std::string wallMat;                 ///< main lead material
  std::string steelMat;                ///< steel material

 public:

  WallLeadGenerator();
  WallLeadGenerator(const WallLeadGenerator&);
  WallLeadGenerator& operator=(const WallLeadGenerator&);
  ~WallLeadGenerator();

  void setWidth(const double,const double);
  void setXOffset(const double);
  
  void generateWall(FuncDataBase&,const std::string&,const double) const;

};

}

#endif
 
