/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/SlitsMaskGenerator.h
 *
 * Copyright (c) 2004-2024 by Konstantin Batkov
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
#ifndef setVariable_SlitsMaskGenerator_h
#define setVariable_SlitsMaskGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class SlitsMaskGenerator
  \version 1.0
  \author Konstantin Batkov
  \date October 2024
  \brief SlitsMaskGenerator for variables
*/

class SlitsMaskGenerator
{
 private:

  double slitLength;            ///< Slit length
  double slitWidth;             ///< Slit width
  double slitHeight;            ///< Slit height

  double wallThick;             ///< wall thickness
  double portRadius;            ///< port radius
  double frontPortLength;       ///< front port length (centre to flange end)
  double backPortLength;            ///< back port length (centre to flange end)
  double leftPortLength;        ///< left port length (centre to flange end)
  double rightPortLength;       ///< right port length (centre to flange end)
  double bottomPortLength;      ///< bottom port length (centre to flange end)
  double topPortLength;         ///< top port total length (centre to flange end)
  double outerFlangeRadius;     ///< outer flange radius
  double outerFlangeThick;      ///< outer flange thickness
  double outerFlangeCapThick;   ///< outer flange cap thickness
  std::string leftFlangeCapWindowMat;  ///< left flange cap window material

  std::string slitsMat;         ///< Main material
  std::string wallMat;          ///< wall material
  std::string voidMat;          ///< void material

 public:

  SlitsMaskGenerator();
  SlitsMaskGenerator(const SlitsMaskGenerator&);
  SlitsMaskGenerator& operator=(const SlitsMaskGenerator&);
  virtual ~SlitsMaskGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
