/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacility/variables.cxx
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
#include <cassert>
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "DuctGenerator.h"
#include "CFFlanges.h"
#include "PipeGenerator.h"
#include "BellowGenerator.h"
#include "SolenoidGenerator.h"
#include "GTFGateValveGenerator.h"
#include "CurrentTransformerGenerator.h"
#include "PipeTubeGenerator.h"
#include "PortItemGenerator.h"
#include "FlangePlateGenerator.h"

namespace setVariable
{
  void BuildingBVariables(FuncDataBase& Control, const std::string& name)
    /*!
      Set Gun Test Facility Building variables
      \param Control :: Database to use
      \param name :: name prefix
    */
  {
    // References:
    // [0] Bjorn's drawings (have higher priority than other refs)
    // http://localhost:8080/maxiv/work-log/100hz/gtf/guntestfacility-bjornnilsson.djvu/view
    // [1] 221413_radiation_caclulation.STEP
    // [2] K_20-1_08C6b4
    // [3] K_20-1_08C6a3
    // [4] self-measured
    // [5] K_20-1_09C6b4
    // [6] K_20-6_015
    // [7] K_20-2_305

    constexpr double length = 650; // [1]
    constexpr double length2 = length/2.0;
    constexpr double width = 460.0; // [1]
    constexpr double width2 = width/2.0;
    constexpr double depth  = 132.1; // [4]
    constexpr double backWallThick = 100.0;  // [1]
    constexpr double outerWallThick = 40.0; // [1] [2]
    constexpr double mazeWidth = 100.0;  // [1] [2]

    Control.addVariable(name+"GunRoomLength",length);
    Control.addVariable(name+"GunRoomWidth",width);
    Control.addVariable(name+"KlystronRoomWidth",460.0); // [1]
    Control.addVariable(name+"InternalWallThick",20.0); // [1]
    Control.addVariable(name+"Depth",depth); // [1]
    Control.addVariable(name+"EastClearance",5.0); // [3]
    Control.addVariable(name+"Height",300.0-depth); // [1]
    Control.addVariable(name+"HallHeight",370.0-depth); // [6]
    Control.addVariable(name+"BackWallThick",backWallThick);
    Control.addVariable(name+"BackWallCornerCut",3.5);
    Control.addVariable(name+"GunRoomEntranceWidth",160.0); // [1] [2]
    Control.addVariable(name+"MidWallThick",100.0); // [1]
    Control.addVariable(name+"OuterWallThick",outerWallThick);
    Control.addVariable(name+"WallMat","Concrete"); // guess
    Control.addVariable(name+"MazeWidth",mazeWidth);
    Control.addVariable(name+"MazeEntranceOffset",4.5); // [0], page 6, but [2]: 4250-3800-400=5 cm
    Control.addVariable(name+"MazeEntranceWidth",100.0); // [0]: 100, but [2]: 101 (outer)
    Control.addVariable(name+"MazeEntranceHeight",210.0); // [0], page 6, but [2,6]: ÖK 7816-7605 = 211 cm
    Control.addVariable(name+"HallLength",500.0); // [2] 10900-5700-200 = 500 cm
    Control.addVariable(name+"FloorThick",60.0); // [6]
    Control.addVariable(name+"RoofGunTestThick",100.0); // [0]
    Control.addVariable(name+"TRSPRoomWidth",415.0); // [2] and [3]
    Control.addVariable(name+"StairRoomWidth",550.0); // [3]: 1450+4050 = 550 cm
    Control.addVariable(name+"StairRoomLength",440.0); // [3]: 1600 + 2800 = 440 cm
    Control.addVariable(name+"ElevatorWidth",250.0); // [3]: 1450+1050 = 250 cm
    Control.addVariable(name+"ElevatorLength",180.0); // [3]: 180 cm
    Control.addVariable(name+"OilRoomEntranceWidth",136.0); // [4]
    Control.addVariable(name+"OilRoomWallThick",10.0); // [4]
    Control.addVariable(name+"OilRoomWallMat","Void"); // gips+wood+void

    Control.addVariable(name+"ControlRoomWidth",850.0); // [4]
    Control.addVariable(name+"ControlRoomLength",292.4); // [4]
    Control.addVariable(name+"ControlRoomWallThick",20.0); // TODO wrong. See [3]
    Control.addVariable(name+"ControlRoomEntranceWidth",211.0); // [3]
    Control.addVariable(name+"ControlRoomEntranceOffset",20.0); // [3] 1450-(9140+2110-10000) mm

    Control.addVariable(name+"Level9Height",270.0); // [6]
    Control.addVariable(name+"Level9RoofThick",30.0); // [6]
    Control.addVariable(name+"Level9VentillationDuctShieldLength",60.0); // [0], page 1
    Control.addVariable(name+"Level9VentillationDuctShieldWidth",220.0); // [0], page 1
    Control.addVariable(name+"Level9VentillationDuctShieldHeight",60.0); // [0], page 6
    Control.addVariable(name+"Level9VentillationDuctShieldThick",5.0); // [4] and [0], page 1
    Control.addVariable(name+"Level9VentillationDuctShieldMat", "Stainless304"); // guess TODO
    Control.addVariable(name+"Level9VentillationDuctShieldOffset",308.0); // [0], page 1

    setVariable::DuctGenerator DuctGen;
    DuctGen.setSize(10.0); // [0], page 4 (diameter 20.0)
    DuctGen.generate(Control,name+"DuctWave",90.0, 0.0, length2-136.5, 269-depth); //  [4]

    DuctGen.setSize(5.0); // [0], page 4
    DuctGen.generate(Control,name+"DuctSignal1",90.0, 0.0, length2-161.5, 279.0-depth); // [4]
    DuctGen.generate(Control,name+"DuctSignal2",90.0, 0.0, length2-191.5+5, 278.5-depth); // [4]

    DuctGen.setSize(8.0); // [4]
    DuctGen.generate(Control,name+"DuctSignal3",90.0, 0.0, length2-189.0, 11.5-depth); // [0], pages 2, 4
    DuctGen.generate(Control,name+"DuctWater1",90.0, 0.0, length2-219.0+3, 11.5-depth-2.0); // [4]

    DuctGen.setSize(10.0); // [0], page 4
    DuctGen.generate(Control,name+"DuctVentillationPenetration",90.0,
		     0.0, length2+backWallThick+77.0, 224.5-depth); //[0], pages 2, 4

    DuctGen.setSize(8.0); // [4]
    DuctGen.generate(Control,name+"DuctLaser",0.0,
		     width2+outerWallThick-55.0,0.0, 263.0-depth+7.0); // [4]

    constexpr double duct6width = 39.7; // [0], page 3
    constexpr double duct6height = 10.0; // [0], page 3
    DuctGen.setSize(duct6width,duct6height);
    DuctGen.generate(Control,name+"DuctSignal4",0.0,
		     185.0-width2-outerWallThick, 0.0,
		     257.5-depth+duct6height/2.0); //[0], pages 2, 4

    constexpr double duct7width = 56.0; // [0], page 3
    constexpr double duct7height = 19.0; // [0], page 3
    DuctGen.setSize(duct7width,duct7height);
    DuctGen.generate(Control,name+"DuctWater2",0.0,
		     114.0-width2-outerWallThick, 0.0,
		     271.0-depth+duct7height/2.0); //[0], pages 2, 4

    DuctGen.setSize(11.25); // [0], page 3, but [7]: diam = 225 mm => R = 11.25 cm, AR measured: close to 11.25
    DuctGen.generate(Control,name+"DuctSuctionFan",0.0,
		     55.0-width2-outerWallThick, 0.0,
		     280.0-depth); //[0], pages 2 and 3

    DuctGen.setSize(22.0); // [0], page 1
    DuctGen.generate(Control,name+"DuctVentillationRoof1",0.0,
		     30.0-width2, 0.0, // [0] page 1
		     length2+backWallThick+mazeWidth-370.0); //[0], page 2
    DuctGen.generate(Control,name+"DuctVentillationRoof2",0.0,
		     30.0-width2, 0.0, // [0] page 1
		     length2+backWallThick+mazeWidth-370.0-120.0); //[0], page 2


    Control.addVariable(name+"ConcreteDoorDoorMat","Concrete");
    Control.addVariable(name+"ConcreteDoorCornerCut",3.0); // measured
    Control.addVariable(name+"ConcreteDoorJambCornerCut",3.5); // measured
    Control.addVariable(name+"ConcreteDoorOuterWidth",153.1); // measured
    Control.addVariable(name+"ConcreteDoorInnerWidth",136.83); // set to have coorect distance of 6.3 cm to the outside right
    Control.addVariable(name+"ConcreteDoorInnerThick",20.0);
    Control.addVariable(name+"ConcreteDoorInnerXStep",10.0); // measured
    Control.addVariable(name+"ConcreteDoorInnerHeight",194.9); // measured
    Control.addVariable(name+"ConcreteDoorOuterHeight",205.0); // measured
    Control.addVariable(name+"ConcreteDoorInnerSideAngle",72.677); // adjusted to get 130.5 cm between corners a and b
    Control.addVariable(name+"ConcreteDoorOuterSideAngle",78.67);
    Control.addVariable(name+"ConcreteDoorUnderStepHeight",7.0); // measured
    Control.addVariable(name+"ConcreteDoorUnderStepWidth",110.5); // somewhat average of measured: 110.2 and 110.6
    Control.addVariable(name+"ConcreteDoorUnderStepXStep",1.4); //

    Control.addVariable(name+"ConcreteDoorInnerSideGapLeft",3.0); // adjusted to have 8.5 cm gap to the jamb
    Control.addVariable(name+"ConcreteDoorInnerSideGapRight",1.17); // adjusted to have total inner jamb width of 141 cm
    Control.addVariable(name+"ConcreteDoorOuterSideGapLeft",7.7); // adjusted to have 4 cm gap to the jamb
    Control.addVariable(name+"ConcreteDoorOuterSideGapRight",2.0); // should be -0.8 to match total width of 1.6 m TODO: check

    Control.addVariable(name+"ConcreteDoorInnerThickGap",0.5); // TODO check

    Control.addVariable(name+"ConcreteDoorInnerTopGap",4.3); // 199.2(total)-194.9(InnerHeight)
    Control.addVariable(name+"ConcreteDoorOuterTopGap",4.5);

    Control.addVariable(name+"ConcreteDoorFlipX",1);
  }

  void BeamLineVariables(FuncDataBase& Control)
  /*!
    Set Gun Test Facility Building variables
    \param Control :: Database to use
    \param name :: name prefix
  */
  {
    Control.addVariable("GTFLineOuterLeft", 100.0);

    std::string name = "IonPumpA";

    // Ion Pump produced by Gamma Vacuum
    Control.addVariable(name+"YStep",24.1);
    Control.addVariable(name+"Length",17.2);
    Control.addVariable(name+"Height",13.0);
    Control.addVariable(name+"WallThick",1.3);
    Control.addVariable(name+"MainMat","Void");
    Control.addVariable(name+"WallMat","Stainless304L"); // dummy TODO
    Control.addVariable(name+"PistonMat","Stainless304"); // dummy TODO

    Control.addVariable(name+"PistonWidth",26.3);
    Control.addVariable(name+"PistonHeight",5.0);
    Control.addVariable(name+"PistonBaseHeight",12.0);
    Control.addVariable(name+"PistonBaseThick",4.5);
    Control.addVariable(name+"PistonLength",15.0);
    Control.addVariable(name+"FlangeRadius", 7.5);
    Control.addVariable(name+"FlangeThick", 2.1);
    Control.addVariable(name+"FlangeTubeRadius", 5.1); // CF100? TODO: check radius
    Control.addVariable(name+"FlangeTubeThick", 0.2);
    Control.addVariable(name+"FlangeTubeLength", 6.9);

    Control.copyVarSet("IonPumpA", "IonPumpB");
    Control.addVariable("IonPumpBYStep",0.0);

    name = "Extension";
    setVariable::PipeGenerator PipeGen;

    PipeGen.setNoWindow();   // no window
    PipeGen.setCF<setVariable::CF100>();
    PipeGen.generatePipe(Control,name,11.0);
    Control.addVariable(name+"PipeThick", 0.2);

    name = "PipeA";
    PipeGen.setCF<setVariable::CF63>(); // dummy TODO - is it a conic pipe?
    PipeGen.generatePipe(Control,name,40.0);
    Control.addVariable(name+"PipeThick", 0.2);

    name = "Solenoid";
    setVariable::SolenoidGenerator SolGen;
    SolGen.generate(Control,name);
    Control.addVariable(name+"YStep", 5.0); // dummy TODO

    // 48.2 XHV All-Metal Gate Valve
    // https://www.vatvalve.com/series/extreme-high-vacuum-all-metal-gate-valve-DN-63-100-160?_locale=en&region=SE
    name = "Gate";
    GTFGateValveGenerator GateGen;

    // Gate blade: http://localhost:8080/maxiv/work-log/100hz/gtf/2d-drawings/vat-xhv-gate-valve-48_2-dn63.png/view
    GateGen.setOuter(4.5-0.5, 15.5-1.0, 18.0-0.5+1, 8.2-0.5); // measured (-0.5 to account for wall thick, +1 for the square hutch)
    GateGen.setBladeMat("EN14435"); // email from AMG 240702
    GateGen.setBladeThick(1.09); // email from AMG 240702
    GateGen.setAPortCF<CF63>(); // TODO
    GateGen.setBPortCF<CF63>(); // TODO
    //    GateGen.setPortPairCF<CF40,CF63>(); // inner-outer TODO
    GateGen.generateValve(Control,"Gate",0.0,1);
    Control.addVariable(name+"PortARadius", 3.15); // measured
    Control.addVariable(name+"PortBRadius", 3.15); // measured
    Control.addVariable(name+"PortAThick", 5.8); // measured
    Control.addVariable(name+"PortBThick", 5.8); // measured
    Control.addVariable(name+"ClampMat", "Iron");  // guess TODO
    Control.addVariable(name+"BladeRadius", 3.5); // email from AMG 240702 (calculated based on other measurements)

    name = "PipeB";
    PipeGen.setCF<setVariable::CF63>();
    PipeGen.generatePipe(Control,name,7.9+CF63::flangeLength); // measured
    Control.addVariable(name+"PipeThick", 0.2);
    Control.addVariable(name+"FlangeBLength", 0.0);

    name = "BellowA";
    setVariable::BellowGenerator BellowGen;
    BellowGen.setCF<setVariable::CF63>();
    BellowGen.generateBellow(Control,name,4.0); // measured
    Control.addVariable(name+"PipeThick", 0.2);
    Control.addVariable(name+"FlangeALength", 0.0);
    Control.addVariable(name+"FlangeBLength", 0.0);
    Control.addVariable(name+"BellowStep", 0.8); // measured

    name = "CurrentTransformer";
    setVariable::CurrentTransformerGenerator CMGen;
    CMGen.generate(Control,name);

    name = "LaserChamber";
    constexpr double lcRadius = 5.0; // measured
    constexpr double lcWall = 0.2; // measured
    setVariable::PipeTubeGenerator PipeTubeGen;
    PipeTubeGen.setCF<CF63>();
    PipeTubeGen.setCap();
    PipeTubeGen.generateTube(Control,name,12.0);
    Control.addVariable(name+"Radius",lcRadius);
    Control.addVariable(name+"WallThick",lcWall);
    Control.addVariable(name+"FlangeACapThick",0.0);
    Control.addVariable(name+"FlangeBCapThick",0.0);
    Control.addVariable(name+"FlangeALength",1.3); // measured
    Control.addVariable(name+"FlangeARadius",6.0); // measured
    Control.addVariable(name+"FlangeBLength",0.0);
    Control.addVariable(name+"NPorts",4);

    setVariable::PortItemGenerator PItemGen;
    constexpr double dr = lcRadius-lcWall;
    PItemGen.setCF<setVariable::CF35_TDC>(dr);
    PItemGen.setPlate(setVariable::CF35_TDC::flangeLength,"Stainless304");
    PItemGen.generatePort(Control,name+"Port0",
			  Geometry::Vec3D(-dr,0,0),
			  Geometry::Vec3D(-1,0,0));
    //    Control.addVariable(name+"Port0CapThick",0.0);
    Control.addVariable(name+"Port0FlangeLength",0); // guess TODO
    Control.addVariable(name+"Port0WindowThick",1.0); // guess TODO
    Control.addVariable(name+"Port0WindowRadius",setVariable::CF35_TDC::innerRadius); // guess TODO

    PItemGen.generatePort(Control,name+"Port1",
			  Geometry::Vec3D(dr,0,0),
			  Geometry::Vec3D(1,0,0));
    const std::set<std::string> p1vars = {"FlangeLength", "WindowThick", "WindowRadius"};
    std::for_each(p1vars.begin(), p1vars.end(),
		  [&](const auto &m) {
		    Control.copyVar(name+"Port1"+m,name+"Port0"+m);
		  });

    PItemGen.generatePort(Control,name+"Port2",
			  Geometry::Vec3D(0,0,dr),
			  Geometry::Vec3D(0,0,1));

    PItemGen.setCF<setVariable::CF66_TDC>(dr);
    PItemGen.generatePort(Control,name+"Port3",
			  Geometry::Vec3D(0,0.7,-dr),
			  Geometry::Vec3D(0,0,-1));
    Control.addVariable(name+"Port3Radius",3.3);
    Control.addVariable(name+"Port3OuterVoid",0);
    Control.addVariable(name+"Port3Length",9.5); // measured approx
    Control.addVariable(name+"Port3CapThick",0.0);


    name += "BackPlate";
    setVariable::FlangePlateGenerator FPGen;

    FPGen.setCF<setVariable::CF63>(1.5); // measured
    FPGen.setFlange(lcRadius+lcWall, 1.3); // 1.3 is measured
    FPGen.generateFlangePlate(Control,name);

  }

  void GunTestFacilityVariables(FuncDataBase& Control)
  /*!
    Set Gun Test Facility Building
    \param Control :: Database to use
  */
  {
    ELog::RegMethod RegA("", "GunTestFacilityVariables");

    BuildingBVariables(Control, "BldB");
    BeamLineVariables(Control);

  }
}
