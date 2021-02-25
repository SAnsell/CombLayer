/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   linac/linacMagnetVariables.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell/Konstantin Batkov
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MagnetGenerator.h"


namespace setVariable
{



void
LINACmagnetVariables(FuncDataBase& Control,
		     const std::string& magField)
  /*!
    Function to set the control variables and constants
    \param Control :: Function data base to add constants too
    \param magField :: Field for the magnet
  */
{
  ELog::RegMethod RegA("linacMangnetVariables[F]","LINACmagnetVariables");

  if (magField=="NONE" || magField=="None")
    return;

  // active units : Void space for field
  const std::vector<std::string> MUname
    ({
	"Seg1CMagHA L2SPF1PipeB:Void",
	"Seg1CMagVA L2SPF1PipeB:Void",
	"Seg1CMagHB L2SPF1PipeD:Void",
	"Seg1CMagVB L2SPF1PipeD:Void",
	"Seg1QuadA  L2SPF1PipeD:Void",
	"Seg1CMagHC L2SPF1PipeE:Void",
	"Seg1CMagVC L2SPF1PipeE:Void",

	"Seg2QuadA L2SPF2PipeA:Void",
	"Seg2QuadB L2SPF1PipeB:Void",
	"Seg2QuadC L2SPF1PipeE:Void",
	"Seg2QuadD L2SPF1PipeE:Void",
	"Seg2QuadE L2SPF1PipeE:Void",

        "Seg3DipoleA L2SPF3FlatA:Void",
	"Seg3CMagHA  L2SPF3PipeA:Void",
	"Seg3CMagVA  L2SPF3PipeA:Void",
	"Seg3DipoleB L2SPF3FlatB:Void",

	"Seg4QuadA   L2SPF4PipeB:Void",
	"Seg4SexuA   L2SPF4PipeB:Void",
	"Seg4QuadB   L2SPF4PipeB:Void",
	"Seg4CMagHA  L2SPF4PipeC:Void",
	"Seg4CMagVA  L2SPF4PipeC:Void",

        "Seg5DipoleA L2SPF5FlatA:Void",
	"Seg5DipoleB L2SPF5FlatB:Void",

	"Seg7CMagHA L2SPF7PipeA:Void",
	"Seg7QuadA L2SPF7PipeA:Void",
	"Seg7CMagVA L2SPF7PipeB:Void",

	"Seg9CMagVA L2SPF9PipeA:Void",
	"Seg9CMagHA L2SPF9PipeA:Void",
	"Seg9QuadA L2SPF9PipeB:Void",

	"Seg10QuadA L2SPF10PipeC:Void",
	"Seg10CMagVA L2SPF10PipeC:Void",

	"Seg11QuadA L2SPF11PipeA:Void",
	"Seg11CMagHA L2SPF11PipeB:Void",

	"Seg12DipoleA L2SPF12FlatA:Void",
	"Seg12DipoleB L2SPF12PipeLA:Void L2SPF12FlatB:Void L2SPF12BeamA:MainVoid",
    });

  for(const std::string& Item : MUname)
    Control.pushVariable<std::string>("MagUnitList",Item);


  if (magField=="TDCline" ||
      magField=="TDClineA" ||
      magField=="TDClineB" ||
      magField=="TDClineC"
      )
    {
      const std::vector<std::string> TDCname
	({
	  "Seg13CMagHA L2SPF13PipeA:Void",
	  "Seg13QuadA L2SPF13PipeB:Void",
	  "Seg13SexuA L2SPF13PipeB:Void",
	  "Seg13QuadB L2SPF13PipeB:Void",
	  "Seg13CMagVA L2SPF13PipeC:Void",

	  "Seg14DipoleA TDC14FlatA:Void",
	  "Seg14DipoleB TDC14FlatB:Void",

	  "Seg16Quad TDC16PipeA:Void",
	  "Seg16CMagHA TDC16PipeB:Void",
	  "Seg16CMagVA TDC16PipeB:Void",

	  "Seg18Quad TDC18PipeA:Void",
	  "Seg18CMagHA TDC18PipeB:Void",
	  "Seg18CMagVA TDC18PipeB:Void",

	  "Seg21Quad TDC21PipeA:Void",
	  "Seg21CMagHA TDC21PipeB:Void",
	  "Seg21CMagVA TDC21PipeB:Void",

	  "Seg23Quad TDC23PipeA:Void",
	  "Seg23CMagHA TDC23PipeB:Void",
	  "Seg23CMagVA TDC23PipeB:Void",

	  "Seg24CMagHA TDC24PipeB:Void",
	  "Seg24CMagVA TDC24PipeB:Void",
	  "Seg24Quad TDC24PipeC:Void",

	  "Seg25DipoleA TDC25TriPipeA:Void",
	});
      for(const std::string& Item : TDCname)
	Control.pushVariable<std::string>("MagUnitList",Item);
    }

  // SPF LINE
  if (magField=="SPFline")
    {
      const std::vector<std::string> SPFname
	({
	  "Seg30CMagVA SPF30PipeB:Void",

	  "Seg31Quad SPF31PipeA:Void",
	  "Seg31CMagHA SPF31PipeB:Void",

	  "Seg32DipoleA SPF32FlatA:Void",
	  "Seg32DipoleB SPF32FlatB:Void",

	  "Seg33CMagHA SPF33PipeA:Void",
	  "Seg33QuadA SPF33PipeB:Void",
	  "Seg33SexuA SPF33PipeB:Void",
	  "Seg33QuadB SPF33PipeB:Void",
	  "Seg33CMagVA SPF33PipeC:Void",

	  "Seg34DipoleA SPF34FlatA:Void",
	  "Seg34DipoleB SPF34FlatB:Void",

	  "Seg35QuadA SPF35PipeA:Void",
	  "Seg35QuadB SPF35PipeB:Void",
	  "Seg35CMagHA SPF35PipeB:Void",
	  "Seg35CMagVA SPF35PipeB:Void",

	  "Seg36QuadA SPF36PipeA:Void",
	  "Seg36CMagHA SPF36PipeA:Void",
	  "Seg36CMagVA SPF35PipeA:Void",
	  "Seg36QuadB SPF36PipeA:Void",

	  "Seg43CMagHA SPF43Pipe:Void",
	  // Artificial magnet to kick the beam towards the main beam
	  // dump
	  "Seg43BellowB SPF43BellowB:Void SPF44TriBend:Void",

	  "Seg44CMag SPF44TriBend:BendVoid"
	});
      for(const std::string& Item : SPFname)
	Control.pushVariable<std::string>("MagUnitList",Item);
    }
  MagnetGenerator MUdipole;

  // SEGMENT 1
  MUdipole.generateCorMag(Control,1,"CMagHA",90.0);
  MUdipole.generateCorMag(Control,1,"CMagVA",0.0);
  MUdipole.generateCorMag(Control,1,"CMagHB",90.0);
  MUdipole.generateCorMag(Control,1,"CMagVB",0.0);
  MUdipole.generateQuad(Control,1,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,1,"CMagHC",90.0);
  MUdipole.generateCorMag(Control,1,"CMagVC",0.0);

  // SEGMENT 2

  MUdipole.generateCorMag(Control,2,"QuadA",0.0);
  MUdipole.generateCorMag(Control,2,"QuadB",0.0);
  MUdipole.generateCorMag(Control,2,"QuadC",0.0);
  MUdipole.generateCorMag(Control,2,"QuadD",0.0);
  MUdipole.generateCorMag(Control,2,"QuadE",0.0);

  // SEGMENT 3
  MUdipole.generateDipole(Control,3,"DipoleA",-90.0,0.8575);  // 0.86
  MUdipole.generateCorMag(Control,3,"CMagHA",90.0);
  MUdipole.generateCorMag(Control,3,"CMagVA",0.0);
  MUdipole.generateDipole(Control,3,"DipoleB",-90.0,0.8625);   // 0.865

  // SEGMENT 4
  MUdipole.generateQuad(Control,4,"QuadA",0.0,0.0);
  MUdipole.generateSexupole(Control,4,"SexuA",0.0,0.0);
  MUdipole.generateQuad(Control,4,"QuadB",0.0,0.0);
  MUdipole.generateCorMag(Control,4,"CMagHA",90.0);
  MUdipole.generateCorMag(Control,4,"CMagVA",0.0);

  // SEGMENT 5
  MUdipole.setSize(65.0,15.0,3.0);
  MUdipole.setField(0.859,0,0,0);
  MUdipole.generate(Control,"Seg5DipoleA","L2SPF5DipoleA","0",-90.0);
  MUdipole.setField(0.859,0,0,0);  // 0.66 : 0.8
  MUdipole.generate(Control,"Seg5DipoleB","L2SPF5DipoleB","0",-90.0);

  // SEGMENT 6

  // // SEGMENT 7
  MUdipole.generateCorMag(Control,7,"CMagHA",90.0);
  MUdipole.generateQuad(Control,7,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,7,"CMagVA",0.0);

  // // SEGMENT 9
  MUdipole.generateCorMag(Control,9,"CMagVA",0.0);
  MUdipole.generateCorMag(Control,9,"CMagHA",90.0,0.00);
  MUdipole.generateQuad(Control,9,"QuadA",0.0,0.0);

  // SEGMENT 10
  MUdipole.generateQuad(Control,10,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,10,"CMagVA",90.0,0.0);

  // SEGMENT 11
  MUdipole.generateQuad(Control,11,"QuadA",0.0,0.0);
  MUdipole.generateCorMag(Control,11,"CMagHA",90.0,0.0);

  if (magField=="SPFline")
    {
      // SEGMENT 12 [SPF]
      MUdipole.generateDipole(Control,12,"DipoleA",0.0,0.0);
      MUdipole.generateDipole(Control,12,"DipoleB",0.0,0.0);
    }
  else
    {
      // SEGMENT 12 [TDC]
      MUdipole.generateDipole(Control,12,"DipoleA",90.0,0.78);
      MUdipole.setOffset(Geometry::Vec3D(0,-10.0,0));
      MUdipole.generateDipole(Control,12,"DipoleB",-90.0,0.945);

      // SEGMENT 13
      MUdipole.generateCorMag(Control,13,"CMagHA",90.0);
      MUdipole.generateQuad(Control,13,"QuadA",0.0,0.0);
      MUdipole.generateSexupole(Control,13,"SexuA",0.0,0.0);
      MUdipole.generateQuad(Control,13,"QuadB",0.0,0.0);
      MUdipole.generateCorMag(Control,13,"CMagVA",0.0);

      // SEGMENT 14
      MUdipole.setPreName("TDC");
      MUdipole.generateDipole(Control,14,"DipoleA",90.0,0.855);
      MUdipole.generateDipole(Control,14,"DipoleB",90.0,0.855);

      // SEGMENT 15

      // SEGMENT 16
      MUdipole.generateQuad(Control,16,"Quad",0.0,0.0);
      MUdipole.generateCorMag(Control,16,"CMagHA",90.0);
      MUdipole.generateCorMag(Control,16,"CMagVA",0.0);

      // SEGMENT 17

      // SEGMENT 18
      MUdipole.generateQuad(Control,18,"Quad",0.0,0.0);
      MUdipole.generateCorMag(Control,18,"CMagHA",0.0);
      MUdipole.generateCorMag(Control,18,"CMagVA",0.0);

      // SEGMENT 19
      // SEGMENT 20

      // SEGMENT 21
      MUdipole.generateQuad(Control,21,"Quad",0.0,0.0);
      MUdipole.generateCorMag(Control,21,"CMagHA",90.0);
      MUdipole.generateCorMag(Control,21,"CMagVA",0.0);

      // SEGMENT 23
      MUdipole.generateQuad(Control,23,"Quad",0.0,0.0);
      MUdipole.generateCorMag(Control,23,"CMagHA",90.0);
      MUdipole.generateCorMag(Control,23,"CMagVA",0.0);

      // SEGMENT 24
      MUdipole.generateCorMag(Control,24,"CMagHA",0.0);
      MUdipole.generateCorMag(Control,24,"CMagVA",0.0);
      MUdipole.generateQuad(Control,24,"Quad",0.0,0.0);

      // SEGMENT 25
      // The beam normally is not deflected by this magnet [email from EM]
      if (magField=="TDCline" || magField=="TDClineA")
	MUdipole.generateDipole(Control,25,"DipoleA",90.0,0.0);
      else if (magField=="TDClineB")
	MUdipole.generateDipole(Control,25,"DipoleA",90.0,0.81);
      else if (magField=="TDClineC")
	MUdipole.generateDipole(Control,25,"DipoleA",90.0,1.59);

      // SEGMENT 26
      // SEGMENT 27
      // SEGMENT 28
      // SEGMENT 29
    }

  // SPFLINE
  // SEGMENT 30
  if (magField=="SPFline")
    {
      MUdipole.setPreName("SPF");
      MUdipole.generateCorMag(Control,30,"CMagVA",0.0);

      // SEGMENT 31
      MUdipole.generateQuad(Control,31,"Quad",0.0,0.0);
      MUdipole.generateCorMag(Control,31,"CMagHA",90.0);

      // SEGMENT 32
      MUdipole.generateDipole(Control,32,"DipoleA",90.0,0.860);
      MUdipole.generateDipole(Control,32,"DipoleB",90.0,0.860);

      // SEGMENT 33
      MUdipole.generateCorMag(Control,33,"CMagHA",90.0);
      MUdipole.generateQuad(Control,33,"QuadA",0.0,0.0);
      MUdipole.generateSexupole(Control,33,"SexuA",0.0,0.0);
      MUdipole.generateQuad(Control,33,"QuadB",0.0,0.0);
      MUdipole.generateCorMag(Control,33,"CMagVA",0.0);


      // SEGMENT 34
      MUdipole.generateDipole(Control,34,"DipoleA",90.0,0.858);
      MUdipole.generateDipole(Control,34,"DipoleB",90.0,0.858);

      // SEGMENT 35
      MUdipole.generateQuad(Control,35,"QuadA",0.0,0.0);
      MUdipole.generateQuad(Control,35,"QuadB",0.0,0.0);
      MUdipole.generateCorMag(Control,35,"CMagHA",90.0);
      MUdipole.generateCorMag(Control,35,"CMagVA",0.0);

      // SEGMENT 36
      MUdipole.generateQuad(Control,36,"QuadA",0.0,0.0);
      MUdipole.generateCorMag(Control,36,"CMagHA",90.0);
      MUdipole.generateCorMag(Control,36,"CMagVA",0.0);
      MUdipole.generateQuad(Control,36,"QuadB",0.0,0.0);


      // SEGMENT 37/38/39/40/41/42


      // SEGMENT 43
      MUdipole.generateCorMag(Control,43,"CMagHA",90.0);
      // Artificial magnet to kick the beam towards the main beam dump
      MUdipole.generateDipole(Control,43,"BellowB",0.0,8.5);
      // We resize it in order to deflect the beam towards
      // the entrance to the curved pipe of SPF44TriBend
      Control.addVariable("MagUnitSeg43BellowBLength",10.0);

      // SEGMENT 44 (curved dipole magnet)
      // We shift and rotate it so that the magnetic field
      // is present in the curved pipe of SPF44TriBend only
      MUdipole.setOffset(0.0,147.0,-41.0);
      MUdipole.generateDipole(Control,44,"CMag",0.0,-3.635);
      Control.addVariable("MagUnitSeg44CMagLength",100.0);
      Control.addVariable("MagUnitSeg44CMagHeight",24.9);
      Control.addVariable("MagUnitSeg44CMagXAngle",-16.5);


      // SEGMENT 45
      // SEGMENT 46
      // SEGMENT 47
      // SEGMENT 48
      // SEGMENT 49
    }

  return;
}

} // NAMESPACE setVariable
