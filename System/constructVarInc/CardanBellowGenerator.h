/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/CardanBellowGenerator.h
 *
 * Copyright (c) 2026 by U. Friman-Gayer
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
#ifndef setVariable_CardanBellowGenerator_h
#define setVariable_CardanBellowGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CardanBellowGenerator
  \version 0.1
  \author U. Friman-Gayer
  \date February 2026
  \brief CardanBellowGenerator for variables
*/

class CardanBellowGenerator
{
 private:

  double angle;
  double bellowStep;
  double bellowThick;
  double bellowsVolumeFraction;
  double flangeLength;
  double flangeRadius;
  double length;
  double pipeInnerRadius;
  double pipeWallThick;

  int nSectors;

  std::string bellowBaseMat;
  std::string pipeMat;

  double bellowLength() const;
  double bellowSheetVolumeOverPi(const int,const double) const;

 public:

  CardanBellowGenerator();
  ~CardanBellowGenerator()=default;

  void setAngle(const double a){angle = a;}
  template<typename CF> void setCF();
  void setNSectors(const int n){nSectors = n;}
  void setMat(const std::string mat){
    bellowBaseMat = mat; bellowsVolumeFraction = 0.0;};
  void setMat(const std::string mat, const double volumeFraction){
    bellowBaseMat = mat; bellowsVolumeFraction = volumeFraction;};
  void setMat(const std::string, const int, const double);

  void generateBellows(
    FuncDataBase&,const std::string&) const;

};

}

#endif
