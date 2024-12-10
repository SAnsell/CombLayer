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

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< Height

  double chamberLengthBack;     ///< chamber length back
  double chamberLengthFront;    ///< chamber length front
  double chamberDepth;          ///< chamber depth
  double chamberHeight;         ///< chamber height
  double chamberWidth;          ///< chamber width
  double chamberWallThick;      ///< chamber wall thickness
  double portRadius;            ///< port radius
  double portThick;             ///< port thick
  double frontLength;           ///< front length (centre to flange end)
  double backLength;            ///< back length (centre to flange end)
  double leftPortLength;        ///< left port total length
  double rightPortLength;       ///< right port total length
  double bottomPortLength;      ///< bottom port total length
  double topPortLength;         ///< top port total length

  std::string slitsMat;          ///< Main material
  std::string chamberMat;       ///< chamber wall material
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
