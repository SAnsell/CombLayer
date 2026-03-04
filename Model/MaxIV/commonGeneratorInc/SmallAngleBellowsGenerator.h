/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/SmallAngleBellowsGenerator.h
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
#ifndef setVariable_SmallAngleBellowsGenerator_h
#define setVariable_SmallAngleBellowsGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SmallAngleBellowsGenerator
  \version 0.1
  \author U. Friman-Gayer
  \date February 2026
  \brief SmallAngleBellowsGenerator for variables
*/

class SmallAngleBellowsGenerator
{
 private:

  double angle;
  double bellowStep;
  double bellowsMaterialThick;
  double bellowThick;
  double bellowsVolumeFraction;
  double flangeLength;
  double flangeRadius;
  double length;
  double pipeInnerRadius;
  double pipeWallThick;

  int nFolds;
  int nSectors;

  std::string bellowBaseMat;
  std::string pipeMat;

 public:

  SmallAngleBellowsGenerator();
  ~SmallAngleBellowsGenerator()=default;

  void setAngle(const double a){angle = a;}
  template<typename CF> void setCF();
  void setNFolds(const int n){nFolds = n;}
  void setNSectors(const int n){nSectors = n;}
  void setBellowThick(const double t){bellowThick = t;}
  void setBellowMaterialThick(const double t){bellowsMaterialThick = t;}
  void setBellowBaseMat(const std::string mat){bellowBaseMat = mat;}

  void generateBellows(
    FuncDataBase&,const std::string&) const;

};

}

#endif
