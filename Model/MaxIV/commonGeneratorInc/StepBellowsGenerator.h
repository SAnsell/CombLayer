/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   constructVarInc/StepBellowsGenerator.h
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
#ifndef setVariable_StepBellowsGenerator_h
#define setVariable_StepBellowsGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class StepBellowsGenerator
  \version 0.1
  \author U. Friman-Gayer
  \date May 2026
  \brief StepBellowsGenerator for variables

  Although the StepBellows class allows for more general configurations, this generator
  assumes that almost all parameters of the front and back bellows are the same.
  The nontrivial parameters are:

  * angle: An "angle" variable like in the SmallAngleBellows class does not exist,
    because the angle is calculated from the step and and the length.
  * nFolds: The number of folds is assumed to hold for the entire structure, and it is
    split half/half between the two SmallAngleBellows. If an odd number is given, the
    value is rounded (integer divide by 2), i.e. nFolds = 11 will result in the front
    and back bellows both having 5 folds.
  * useFrontPipe/useBackPipe: It is assumed that the central connection does not use
    pipe adapters, but it is possible to switch off the pipe adapters at the front of
    the front bellows and at the back of the back bellows.
*/

class StepBellowsGenerator
{
 private:

  double bellowsStep;
  double bellowsMaterialThick;
  double bellowsThick;
  double flangeLength;
  double flangeRadius;
  double length;
  double pipeInnerRadius;
  double pipeWallThick;
  double step;

  int nFolds;
  int nSectors;

  std::string bellowsBaseMat;
  std::string pipeMat;

  int useFrontPipe;
  int useBackPipe;

 public:

  StepBellowsGenerator();
  StepBellowsGenerator(const double s, const double l);
  ~StepBellowsGenerator()=default;

  template<typename CF> void setCF();
  void setBellowsThick(const double t){bellowsThick = t;}
  void setBellowsMaterialThick(const double t){bellowsMaterialThick = t;}
  void setBellowsBaseMat(const std::string mat){bellowsBaseMat = mat;}
  void setLength(const double l){length = l;}
  void setNFolds(const int n){nFolds = n;}
  void setNSectors(const int n){nSectors = n;}
  void setStep(const double s){step = s;}
  void setPipes(const bool useFront, const bool useBack){
    useFrontPipe=useFront;
    useBackPipe=useBack;
  }

  void generateBellows(
    FuncDataBase&,const std::string&) const;

};

}

#endif
