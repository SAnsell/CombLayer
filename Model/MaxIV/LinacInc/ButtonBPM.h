/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/ButtonBPM.h
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
#ifndef tdcSystem_ButtonBPM_h
#define tdcSystem_ButtonBPM_h

class Simulation;

namespace tdcSystem
{

/*!
  \class ButtonBPM
  \version 1.0
  \author Konstantin Batkov
  \date June 2020
  \brief Button pickup BPM
*/

class ButtonBPM :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap,
    public attachSystem::SurfMap,
    public attachSystem::FrontBackCut
{
 private:

  double length;                ///< Total length including void
  double innerRadius;           ///< Inner radius
  double outerRadius;           ///< Outer radius
  size_t nButtons;              ///< Number of buttons [2 or 4]

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
  int buttonCaseMat;            ///< Button case material
  double buttonHandleRadius;    ///< Button handle radius
  double buttonHandleLength;    ///< Button handle length
  double elThick;               ///< Electrode thickness
  double elGap;                 ///< Void gap around electode
  double elCase;                ///< Case thickness around electrode
  double elRadius;              ///< Electrode radius
  int elMat;                    ///< Electrode material
  double ceramicThick;          ///< Ceramic thickness
  int ceramicMat;               ///< Ceramic material
  double pinRadius;             ///< Pin radius [smaller]
  double pinOuterRadius;        ///< Pin radius [larger]
  int pinMat;                   ///< Pin material

  int voidMat;                  ///< Void material
  int wallMat;                  ///< Wall material
  int flangeMat;                ///< Flange material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  ButtonBPM(const std::string&);
  ButtonBPM(const ButtonBPM&);
  ButtonBPM& operator=(const ButtonBPM&);
  virtual ButtonBPM* clone() const;
  virtual ~ButtonBPM();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
