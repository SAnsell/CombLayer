/*********************************************************************
  CombLayer : MCNP(X) Input builder

  * File:   commonGeneratorInc/MainBeamDumpGenerator.h
  *
  * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef setVariable_MainBeamDumpGenerator_h
#define setVariable_MainBeamDumpGenerator_h

class FuncDataBase;

namespace setVariable
{
  /*!
    \class MainBeamDumpGenerator
    \version 1.0
    \author K. Batkov
    \date June 2023
    \brief MainBeamDumpGenerator for variables
  */

  class MainBeamDumpGenerator
  {
  private:

    double width;                 ///< void radius
    double length;                ///< void length [total]
    double height;                ///< void height
    double depth;                 ///< void depth

    double wallThick;             /// main wall thickness
    double portLength;            ///< port length
    double portRadius;            ///< port radius
    double targetWidth;           ///< target width
    double targetHeight;          ///< target height
    double targetLength;          ///< target length

    std::string mat;              ///< main material
    std::string voidMat;          ///< void material
    std::string wallMat;          ///< wall material
    std::string targetMat;        ///< target material

  public:

    MainBeamDumpGenerator();
    MainBeamDumpGenerator(const MainBeamDumpGenerator&);
    MainBeamDumpGenerator& operator=(const MainBeamDumpGenerator&);
    virtual ~MainBeamDumpGenerator();

    virtual void generate(FuncDataBase&,const std::string&) const;

  };

}

#endif
