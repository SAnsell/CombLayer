/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoShutterR3Generator.h
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
#ifndef setVariable_MonoShutterR3Generator_h
#define setVariable_MonoShutterR3Generator_h

class FuncDataBase;

namespace setVariable
{
  class FlangePlateGenerator;
  class PipeTubeGenerator;
  class PortItemGenerator;
  class ShutterUnitGenerator;

/*!
  \class MonoShutterR3Generator
  \version 1.1
  \author Udo Friman-Gayer
  \date Feburary 2026
  \brief MonoShutterR3Generator for variables

  Version history:
  1.1 - 2026-11-02
    - Update for MonoShutterR3 v1.0
  1.0 - 2026-10-02
*/
template<typename MainFlange,typename EntryExitFlange,
typename ShutterFlange,typename AdapterFlange>
class MonoShutterR3Generator
{
 private:

  std::unique_ptr<FlangePlateGenerator> FPGen;
  std::unique_ptr<PipeTubeGenerator> PTubeGen;
  std::unique_ptr<PortItemGenerator> PItemGen;
  std::unique_ptr<ShutterUnitGenerator> SUnitGen;

  double height;
  double length;

  double beamPortInnerRadius;
  double beamPortWallThick;
  double beamPortFlangeRadius;
  double beamPortFlangeLength;

  double vesselInnerRadius;
  double vesselWallThick;
  double vesselFlangeRadius;
  double vesselFlangeLength;
  std::string vesselMat;

  double apertureBackLength;
  double apertureInnerRadius;
  std::string apertureMat;
  double apertureOuterRadius;
  double apertureThick;
  double apertureToBlockGap;

  double blockHeight;
  double blockLength;
  double blockWidth;
  std::string blockMat;

  double shutterDistance;
  double shutterPortLength;
  double shutterPortInnerRadius;
  double shutterPortWallThick;
  double shutterPortFlangeRadius;
  double shutterPortFlangeLength;
  double threadLength;
  double threadRadius;
  double lift;

 public:

  MonoShutterR3Generator();
  virtual ~MonoShutterR3Generator();

  double getLength() const {return length;};
    
  void generate(FuncDataBase&,const std::string&,
		       const bool,const bool) const;
  
};

}

#endif
 
