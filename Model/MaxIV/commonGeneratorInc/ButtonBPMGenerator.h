/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/ButtonBPMGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_ButtonBPMGenerator_h
#define setVariable_ButtonBPMGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ButtonBPMGenerator
  \version 1.0
  \author Konstantin Batkov
  \date June 2020
  \brief ButtonBPMGenerator for variables
*/

class ButtonBPMGenerator
{
 private:

  double length;                ///< Total length including void
  double innerRadius;           ///< Inner radius
  double outerRadius;           ///< Outer radius
  int nButtons;                 ///< Number of buttons [2 or 4]

  double flangeInnerRadius;     ///< Flange inner radius
  double flangeALength;         ///< Flange A length
  double flangeARadius;         ///< Flange A radius
  double flangeBLength;         ///< Flange B length
  double flangeBRadius;         ///< Flange B radius
  double flangeGap;             ///< Gap length between flanges and main pipe body

  double buttonYAngle;          ///< Y angle of buttons
  double buttonFlangeRadius;    ///< Button case flange radius
  double buttonFlangeLength;    ///< Button flange length
  double buttonCaseLength;      ///< Button case length
  double buttonCaseRadius;      ///< Button case radius
  std::string buttonCaseMat;    ///< Button case material
  double buttonHandleRadius;    ///< Button handle radius
  double buttonHandleLength;    ///< Button handle length
  double elThick;               ///< Electrode thickness
  double elGap;                 ///< Void gap around electode
  double elCase;                ///< Case thickness around electrode
  double elRadius;              ///< Electrode radius
  std::string elMat;            ///< Electrode material
  double ceramicThick;          ///< Ceramic thickness
  std::string ceramicMat;       ///< Ceramic material
  double pinRadius;             ///< Pin radius [smaller]
  double pinOuterRadius;        ///< Pin radius [larger]
  std::string pinMat;           ///< Pin material

  std::string voidMat;          ///< Void material
  std::string wallMat;          ///< Wall material
  std::string flangeMat;        ///< Flange material

 public:

  ButtonBPMGenerator();
  ButtonBPMGenerator(const ButtonBPMGenerator&);
  ButtonBPMGenerator& operator=(const ButtonBPMGenerator&);
  virtual ~ButtonBPMGenerator();

  template<typename T> void setCF();
  template<typename T> void setAFlangeCF();
  template<typename T> void setBFlangeCF();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
