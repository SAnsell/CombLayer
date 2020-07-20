/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/StriplineBPMGenerator.h
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
#ifndef setVariable_StriplineBPMGenerator_h
#define setVariable_StriplineBPMGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class StriplineBPMGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief StriplineBPMGenerator for variables
*/

class StriplineBPMGenerator
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]

  double outerThick;            ///< pipe thickness

  double innerRadius;           ///< inner radius
  double innerThick;            ///< Inner electorn thickness
  double innerAngle;            ///< Angle of stripline
  double innerAngleOffset;      ///< Offset angle of inner electron

  double flangeARadius;         ///< Joining Flange radius
  double flangeALength;         ///< Joining Flange length

  double flangeBRadius;         ///< Joining Flange radius
  double flangeBLength;         ///< Joining Flange length

  double striplineRadius;       ///< Stripline distance [support]
  double striplineThick;        ///< Stripline thickness [support]
  double striplineYStep;        ///< Stripline YStep
  double striplineEnd;          ///< Stripline end piece length

  std::string voidMat;                  ///< void material
  std::string striplineMat;             ///< stripline material
  std::string flangeMat;                ///< flange material
  std::string outerMat;                 ///< pipe material


 public:

  StriplineBPMGenerator();
  StriplineBPMGenerator(const StriplineBPMGenerator&);
  StriplineBPMGenerator& operator=(const StriplineBPMGenerator&);
  virtual ~StriplineBPMGenerator();

  template<typename T> void setCF();
  template<typename T> void setAFlangeCF();
  template<typename T> void setBFlangeCF();

  virtual void generateBPM(FuncDataBase&,const std::string&,
			   const double) const;

};

}

#endif
