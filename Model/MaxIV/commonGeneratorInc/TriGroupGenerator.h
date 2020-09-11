/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/TriGroupGenerator.h
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
#ifndef setVariable_TriGroupGenerator_h
#define setVariable_TriGroupGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TriGroupGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief TriGroupGenerator for variables
*/

class TriGroupGenerator
{
 private:

  double mainWidth;             ///< void width at front  [inner]
  double mainHeight;            ///< void height [front/inner]
  double mainLength;             ///< void length [main only]
  double mainSideAngle;          ///< void side drop angle (X-rotate)

  double wallThick;              ///< pipe thickness

  double flangeRadius;          ///< Joining Flange radius
  double flangeLength;          ///< Joining Flange length

  double topRadius;                ///< Top radius
  double topLength;                ///< Top length
  double topWallThick;             ///< Top wall thick
  double topFlangeRadius;          ///< Joining Flange radius
  double topFlangeLength;          ///< Joining Flange length


  double midZAngle;                ///< Angle of mid section
  double midOpeningAngle;          ///< mid opening angle (half)
  double midLength;                ///< mid length after main
  double midHeight;                ///< mid void height [full]
  double midWidth;                 ///< mid void width
  double midThick;                 ///< mid wall thick
  double midFlangeRadius;          ///< Joining Flange radius
  double midFlangeLength;          ///< Joining Flange length

  double bendZAngle;               ///< Drop angle of electron path in magnet
  double bendZDrop;                ///< Drop of electron path in magnet
  double bendArcRadius;            ///< Radius of bend section
  double bendArcLength;            ///< bend arc length after main
  double bendStrLength;            ///< straign length after arc
  double bendHeight;               ///< bend void height [full]
  double bendWidth;                ///< bend void width
  double bendThick;                ///< bend wall thick
  double bendFlangeRadius;         ///< Joining Flange radius
  double bendFlangeLength;         ///< Joining Flange length

  std::string voidMat;           ///< Void material
  std::string wallMat;           ///< Pipe material
  std::string flangeMat;         ///< Void material

 public:

  TriGroupGenerator();
  TriGroupGenerator(const TriGroupGenerator&);
  TriGroupGenerator& operator=(const TriGroupGenerator&);
  virtual ~TriGroupGenerator();

  template<typename CF> void setFlangeCF();
  template<typename CF> void setTopFlangeCF();
  template<typename CF> void setMidFlangeCF();
  template<typename CF> void setBendFlangeCF();

  /// accessor to Radius and drop
  void setBend(const double R,const double L,const double SL)
  {  bendArcRadius=R; bendArcLength=L; bendStrLength=SL;}

  void generateTri(FuncDataBase&,const std::string&) const;

};

}

#endif
