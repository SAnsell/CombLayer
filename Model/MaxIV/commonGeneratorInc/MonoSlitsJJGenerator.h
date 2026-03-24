/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoSlitsJJGenerator.h
 *
 * Copyright (c) 2026 by Udo Friman-Gayer
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
#ifndef setVariable_MonoSlitsJJGenerator_h
#define setVariable_MonoSlitsJJGenerator_h

class FuncDataBase;

namespace setVariable
{
/*!
  \class MonoSlitsJJGenerator
  \version 0.1
  \author Udo Friman-Gayer
  \date March 2026
  \brief MonoSlitsJJGenerator for variables
*/
class MonoSlitsJJGenerator
{
 private:

  double length;

  double adapterThick;
  double adapterInnerRadius;

  double auxPortAngleStep;
  double auxPortCenterAngle;
  double auxPortFlangeLength;
  double auxPortFlangeRadius;
  double auxPortInnerRadius;
  double auxPortOpticalAxisOffset;
  double auxPortWallThick;

  double mainInnerRadius;
  double mainWallThick;

  double baseDepth;
  double baseThick;
  double baseWidth;

  double bladeAngle;
  double bladeLongEdge;
  double bladeM1InPos;
  double bladeM1OutPos;
  double bladeM1Pos;
  double bladeM2InPos;
  double bladeM2OutPos;
  double bladeM2Pos;
  double bladeM3InPos;
  double bladeM3OutPos;
  double bladeM3Pos;
  double bladeM4InPos;
  double bladeM4OutPos;
  double bladeM4Pos;
  double bladePortCenterDist;
  double bladePortDist;
  double bladePortFlangeLength;
  double bladePortFlangeRadius;
  double bladePortInnerRadius;
  double bladePortLength;
  double bladePortWallThick;
  double bladeShortEdge;
  double bladeThick;
  double bladeThreadLength;
  double bladeThreadRadius;
  double bladeToThreadDist;

  std::string bladeMat;
  std::string mainMat;

 public:

  MonoSlitsJJGenerator();
  ~MonoSlitsJJGenerator() = default;

  double getLength() const {return length;};

  void generate(
    FuncDataBase&,const std::string&,
    const int m1PosFlag=0,const int m2PosFlag=0,
    const int m3PosFlag=0,const int m4PosFlag=0) const;
  
};

}

#endif
 
