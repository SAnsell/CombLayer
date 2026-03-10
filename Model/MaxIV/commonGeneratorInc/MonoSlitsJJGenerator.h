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

  double mainInnerRadius;
  double mainWallThick;

  double bladeAngle;
  double bladeLongEdge;
  double bladeM1Pos;
  double bladeM2Pos;
  double bladeM3Pos;
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
    
  void generate(FuncDataBase&,const std::string&) const;
  
};

}

#endif
 
