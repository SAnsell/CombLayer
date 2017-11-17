/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/essVariables.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Konstantin Batkov
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"
#include "essVariables.h"

namespace setVariable
{

void
EssVariables(FuncDataBase& Control,
             const std::set<std::string>& beamNames)
  /*!
    Function to set the control variables and constants
    -- This version is for ESS ()
    \param Control :: Function data base to add constants too
    \param beamName :: Set of beamline names
  */
{
  ELog::RegMethod RegA("setVariable","EssVariables");
  
// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  Control.addVariable("EngineeringActive",0);      // NO engineering

  Control.addVariable("TopFocusDistance",8.90);
  Control.addVariable("TopFocusWidth",5.40);
  Control.addVariable("TopFocusXYAngle",90.0);
  Control.addVariable("TopFocusZStep",13.70);

  Control.addVariable("LowFocusXYAngle",90.0);
  Control.addVariable("LowFocusWidth",5.40);
  Control.addVariable("LowFocusDistance",8.90);
  Control.addVariable("LowFocusZStep",-15.20);

  // TOP A FLIGHT
  Control.addVariable("TopAFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopAFlightZStep", 0.0);      // Step from centre

  Control.addVariable("TopAFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("TopAFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("TopAFlightXYAngle",180.0);
  Control.addVariable("TopAFlightZAngle",0.0);
  Control.addVariable("TopAFlightTapSurf","cone");  // Step down angle
  Control.addVariable("TopAFlightAngleZTop",1.0);  // Step down angle
  Control.addVariable("TopAFlightAngleZBase",1.0); // Step up angle


  //  Control.Parse("TopFlyTotalHeight");
  Control.addVariable("TopAFlightHeight", 4.6);     // Full height = TopFlyTotalHeight
  Control.addVariable("TopAFlightWidth", 10.7);     // Full width
  Control.addVariable("TopAFlightNLiner", 1);      // Liner
  Control.addVariable("TopAFlightLinerThick1", 0.4);      // Liner
  Control.addVariable("TopAFlightLinerMat1","Aluminium");      // Liner

  Control.addVariable("TopBFlightXStep",0.0);      // Step from centre
  Control.addVariable("TopBFlightZStep",0.0);      // Step from centre
  Control.addVariable("TopBFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("TopBFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("TopBFlightXYAngle", .0);
  Control.addVariable("TopBFlightZAngle",0.0);
  Control.addVariable("TopBFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("TopBFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("TopBFlightHeight",4.6);     // Full height = TopFlyTotalHeight
  Control.addVariable("TopBFlightWidth", 10.7);     // Full width
  Control.addVariable("TopBFlightNLiner", 1);     
  Control.addVariable("TopBFlightLinerThick1", 0.4); 
  Control.addVariable("TopBFlightLinerMat1","Aluminium");

  
  Control.addVariable("BeRefXStep",0.0);  
  Control.addVariable("BeRefYStep",0.0);  
  Control.addVariable("BeRefZStep",0.0);
  Control.addVariable("BeRefXYAngle",0.0); 
  Control.addVariable("BeRefZAngle",0.0);
  Control.addVariable("BeRefRadius",34.3);
  Control.addVariable("BeRefHeight",74.2);
  Control.addVariable("BeRefWallThick",3.0);
  Control.addVariable("BeRefWallThickLow",0.0);
  Control.addVariable("BeRefTargetSepThick",13.0);
  Control.addVariable("BeRefLowVoidThick",2.3);
  Control.addVariable("BeRefTopVoidThick",2.3);
  Control.addVariable("BeRefLowRefMat","Be5H2O");
  Control.addVariable("BeRefTopRefMat","Be5H2O");
  Control.addVariable("BeRefLowWallMat","Stainless304");
  Control.addVariable("BeRefTopWallMat","Stainless304");
  Control.addVariable("BeRefTargSepMat","Void");

  ///< TODO : Fix double variable dependency !!!
  
  Control.addVariable("BeRefInnerStructureWaterDiscThick", 0.6);
  Control.addVariable("BeRefInnerStructureWaterDiscMat","H2O");
  Control.addVariable("BeRefInnerStructureWaterDiscWallThick", 0.3);
  Control.addVariable("BeRefInnerStructureWaterDiscWallMat", "Aluminium");

  Control.addVariable("BeRefInnerStructureBeRadius", 12.5);
  Control.addVariable("BeRefInnerStructureBeMat", "Be10H2O");
  Control.addVariable("BeRefInnerStructureBeWallThick", 0.3);
  Control.addVariable("BeRefInnerStructureBeWallMat", "Aluminium");

  Control.addVariable("TopBeRefWaterDiscNLayers",2);
  Control.addVariable("TopBeRefWaterDiscHeight0",0.3);
  Control.addVariable("TopBeRefWaterDiscDepth0",0.3);
  Control.Parse("BeRefRadius");
  Control.addVariable("TopBeRefWaterDiscRadius0");
  Control.addVariable("TopBeRefWaterDiscMat0","H2O");
  Control.addVariable("TopBeRefWaterDiscHeight1",0.4);
  Control.addVariable("TopBeRefWaterDiscDepth1",0.0);
  Control.Parse("BeRefWallThick");
  Control.addVariable("TopBeRefWaterDiscRadius1");
  Control.addVariable("TopBeRefWaterDiscMat1", "Aluminium");

  Control.addVariable("LowBeRefWaterDiscNLayers",2);
  Control.addVariable("LowBeRefWaterDiscHeight0",0.3);
  Control.addVariable("LowBeRefWaterDiscDepth0",0.3);
  Control.Parse("BeRefRadius");
  Control.addVariable("LowBeRefWaterDiscRadius0");
  Control.addVariable("LowBeRefWaterDiscMat0","H2O");
  Control.addVariable("LowBeRefWaterDiscHeight1",0.4);
  Control.addVariable("LowBeRefWaterDiscDepth1",0.0);
  Control.Parse("BeRefWallThick");
  Control.addVariable("LowBeRefWaterDiscRadius1");
  Control.addVariable("LowBeRefWaterDiscMat1", "Aluminium");

  Control.addVariable("BulkXStep",0.0);
  Control.addVariable("BulkYStep",0.0);
  Control.addVariable("BulkZStep",0.0);
  Control.addVariable("BulkXYAngle",0.0);
  Control.addVariable("BulkZAngle",0.0);
  Control.addVariable("BulkNLayer",3);

  Control.addParse<double>("BulkRadius1","BeRefRadius+BeRefWallThick+0.2");
  /*!
    \todo : This is ugly conterintuative
    and going to break if anyone make a change
  */

  Control.Parse("BeRefHeight/2.0+BeRefWallThick+"
                "TopBeRefWaterDiscHeight0+TopBeRefWaterDiscDepth0+"
		"TopBeRefWaterDiscHeight1+0.2");
  Control.addVariable("BulkHeight1");
  Control.Parse("BeRefHeight/2.0+BeRefWallThick+LowBeRefWaterDiscHeight0"
		"+LowBeRefWaterDiscDepth0+LowBeRefWaterDiscHeight1+0.2");
  Control.addVariable("BulkDepth1");
  Control.addVariable("BulkMat1","Void");

  Control.addVariable("BulkRadius2",65.0);
  Control.addVariable("BulkHeight2",75.0);
  Control.addVariable("BulkDepth2",75.0);
  Control.addVariable("BulkMat2","Iron10H2O");        // SA: using hand-made mixture because CL can't generate volume fractions

  // Bulk steel layer [No individual guides]
  Control.addVariable("BulkRadius3",200.0);
  Control.addVariable("BulkHeight3",200.0);
  Control.addVariable("BulkDepth3",200.0);
  Control.addVariable("BulkMat3","Iron10H2O");        // SA: using hand-made mixture because CL can't generate volume fractions



  // BULK FLIGHT VOID
  Control.addVariable("BulkLAFlightSideIndex",-2);   // Index
  Control.addVariable("BulkLAFlightXStep",0.0);      // Step from centre
  Control.addVariable("BulkLAFlightZStep",0.0);      // Step from centre
  Control.addVariable("BulkLAFlightAngleXY1",30.0);  // Angle out
  Control.addVariable("BulkLAFlightAngleXY2",30.0);  // Angle out
  Control.addVariable("BulkLAFlightAngleZTop",0.0);  // Step down angle
  Control.addVariable("BulkLAFlightAngleZBase",0.0); // Step up angle
  Control.addVariable("BulkLAFlightHeight",10.0);    // Full height
  Control.addVariable("BulkLAFlightWidth",23.3);     // Full width
  Control.addVariable("BulkLAFlightNLiner",0);       // Liner


  // SHUTTER BAY
  Control.addVariable("ShutterBayRadius",550.0);
  Control.addVariable("ShutterBayTopRadius",500.0);
  Control.addVariable("ShutterBayTopCut",186.0);
  Control.addVariable("ShutterBayHeight",400.0);
  Control.addVariable("ShutterBayDepth",400.0);
  Control.addVariable("ShutterBaySkin",6.0);
  Control.addVariable("ShutterBayTopSkin",6.0);
  Control.addVariable("ShutterBayCutSkin",6.0);
  Control.addVariable("ShutterBayMat","CastIron");

  Control.addVariable("ShutterBaySkinMat","Void");
  Control.addVariable("ShutterBayNCurtain",3);
  Control.addVariable("ShutterBayCurtainMat0","Stainless304");
  Control.addVariable("ShutterBayCurtainMat1","Void");
  Control.addVariable("ShutterBayCurtainMat2","Concrete");
  Control.addVariable("ShutterBayCurtainThick0",70.0);
  Control.addVariable("ShutterBayCurtainThick1",6.0);

  // Guide BAY [ All 4 same ]
  Control.addVariable("GuideBayXStep",0.0);  
  Control.addVariable("GuideBayYStep",0.0);  
  Control.addVariable("GuideBayZStep",0.0);
  Control.addVariable("GuideBayZAngle",0.0);
  Control.addVariable("GuideBayViewAngle",128.0); 
  Control.addVariable("GuideBayInnerHeight",40.0);
  Control.addVariable("GuideBayInnerDepth",40.0);
  Control.addVariable("GuideBayMidRadius",170.0);
  Control.addVariable("GuideBayHeight",50.0);
  Control.addVariable("GuideBayDepth",50.0);
  Control.addVariable("GuideBayMat","CastIron");
  Control.addVariable("GuideBay1XYAngle",270.0); 
  Control.addVariable("GuideBay2XYAngle",90.0); 
  Control.addVariable("GuideBay1NItems",21);  
  Control.addVariable("GuideBay2NItems",21);  


    // Twister
  Control.addVariable("TwisterXStep",11.0);
  Control.addVariable("TwisterYStep",-62.0);
  Control.addVariable("TwisterZStep",0.0);
  Control.addVariable("TwisterXYAngle",10.0);
  Control.addVariable("TwisterZAngle",0.0);
  Control.addVariable("TwisterShaftRadius",18.5);
  Control.addVariable("TwisterShaftHeight",120.0+222.4);
  Control.addVariable("TwisterShaftMat","Iron10H2O");
  Control.addVariable("TwisterShaftWallThick",3.0);
  Control.addVariable("TwisterShaftWallMat","Iron10H2O"); 
  Control.addVariable("TwisterShaftBearingRadius",4);
  Control.addVariable("TwisterShaftBearingHeight",54.4);
  Control.addVariable("TwisterShaftBearingWallThick",12.2);
  Control.addVariable("TwisterPlugFrameRadius",105.0);
  Control.addVariable("TwisterPlugFrameWallThick",3.0);
  Control.addVariable("TwisterPlugFrameHeight",57.6);
  Control.addVariable("TwisterPlugFrameDepth",60.6);
  Control.addVariable("TwisterPlugFrameAngle",38.0);
  Control.addVariable("TwisterPlugFrameMat","Iron10H2O");
  Control.addVariable("TwisterPlugFrameWallThick",3.0);
  Control.addVariable("TwisterPlugFrameWallMat","Iron10H2O");


  // Target Station main building
  Control.addVariable("TSMainBuildingXStep", 0.0);
  Control.addVariable("TSMainBuildingYStep", 0.0);
  Control.addVariable("TSMainBuildingZStep", 0.0);
  Control.addVariable("TSMainBuildingXYangle", 0.0);
  Control.addVariable("TSMainBuildingZangle", 0.0);
  Control.addVariable("TSMainBuildingLength", 2200*2.0);
  Control.addVariable("TSMainBuildingWidth",  1090*2.0);
  Control.addVariable("TSMainBuildingHeight", 1000.0);
  Control.addVariable("TSMainBuildingDepth",  750.0);
  Control.addVariable("TSMainBuildingMainMat",  "Concrete");
  Control.addVariable("TSMainBuildingRoomMat",  "Void");

  EssProtonBeam(Control);
  EssBeamLinesVariables(Control);
  EssPipeVariables(Control);
  
  EssButterflyModerator(Control);
  EssWheel(Control);
  EssBunkerVariables(Control);
  EssIradVariables(Control);
  EssFlightLineVariables(Control);
  F5Variables(Control);

  EssInstrumentVariables(beamNames,Control);
  
  Control.addVariable("sdefEnergy",2000.0);
  // port version:
  Control.addVariable("portSourceASpread",0.0);
  Control.addVariable("portSourceHeight",3.0);
  Control.addVariable("portSourceEStart",3.0);
  Control.addVariable("portSourceEEnd",4.0);
  Control.addVariable("portSourceNE",2);


  // FINAL:
  Control.resetActive();
  return;
}

void
F5Variables(FuncDataBase& Control)
/*!
  Create variables for all F5 collimators
 */
{
    Control.addVariable("F5Length", 440);
    Control.addVariable("F5XB", 10.58);
    Control.addVariable("F5YB", -14.2);
    Control.addVariable("F5ZB", 14.4);
    Control.addVariable("F5XC", 10.47);
    Control.addVariable("F5YC", -8.20);
    Control.addVariable("F5ZC", 14.4);
    Control.addVariable("F5ZG", 11.4);
    Control.addVariable("F5X", 999.84);
    Control.addVariable("F5Y", -17.45);
    Control.addVariable("F5Z", 12.9);

    Control.addVariable("F15Length", 440);
    Control.addVariable("F15XB", 10.58);
    Control.addVariable("F15YB", -14.2);
    Control.addVariable("F15ZB", 14.4);
    Control.addVariable("F15XC", 10.47);
    Control.addVariable("F15YC", -8.20);
    Control.addVariable("F15ZC", 14.4);
    Control.addVariable("F15ZG", 11.4);
    Control.addVariable("F15X", -999.84);
    Control.addVariable("F15Y", -17.45);
    Control.addVariable("F15Z", 12.9);
}
  
void
EssBeamLinesVariables(FuncDataBase& Control)
  /*!
    Create all the beamline variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssBeamLinesVariables");
  const std::string TB[2]={"Top","Low"};
  for(size_t i=0;i<4;i++)
    {
      const std::string baseKey=
        StrFunc::makeString("G",i/2+1)+"BLine"+TB[i%2];
      
      // BeamLine in guide bay
      Control.addVariable(baseKey+"XStep",0.0);  
      Control.addVariable(baseKey+"YStep",0.0);  
      Control.addVariable(baseKey+"ZStep",0.0);
      Control.addVariable(baseKey+"ZAngle",0.0);
      Control.addVariable(baseKey+"Mat","Stainless304");
      Control.addVariable(baseKey+"BeamXYAngle",0.0); 
      Control.addVariable(baseKey+"BeamZAngle",0.0);
      Control.addVariable(baseKey+"BeamXStep",0.0);
      Control.addVariable(baseKey+"BeamZStep",0.0);  
      Control.addVariable(baseKey+"BeamHeight",6.0);
      Control.addVariable(baseKey+"BeamWidth",6.0);
      Control.addVariable(baseKey+"NSegment",2);
      Control.addVariable(baseKey+"SideGap",0.6);
      Control.addVariable(baseKey+"BaseGap",0.1);
      Control.addVariable(baseKey+"TopGap",0.8); 
      Control.addVariable(baseKey+"Width1",20.0);
      Control.addVariable(baseKey+"Depth1",10.0);
      Control.addVariable(baseKey+"Height1",12.0);
      Control.addVariable(baseKey+"Width2",28.0);
      Control.addVariable(baseKey+"Height2",22.0);
      Control.addVariable(baseKey+"Depth2",22.0);
      Control.addVariable(baseKey+"Length1",170.0);
      Control.addVariable(baseKey+"1XYAngle",90.0-30.0);   // N1
      Control.addVariable(baseKey+"2XYAngle",90.0-35.3); 
      Control.addVariable(baseKey+"3XYAngle",90.0-42.0); 
      Control.addVariable(baseKey+"4XYAngle",90.0-47.3); 
      Control.addVariable(baseKey+"5XYAngle",90.0-54.0); 
      Control.addVariable(baseKey+"6XYAngle",90.0-59.3); 
      Control.addVariable(baseKey+"7XYAngle",90.0-66.0); 
      Control.addVariable(baseKey+"8XYAngle",90.0-71.3); 
      Control.addVariable(baseKey+"9XYAngle",90.0-78.0);
      Control.addVariable(baseKey+"10XYAngle",90.0-83.3); 
      Control.addVariable(baseKey+"11XYAngle",90.0-90.0);

      Control.addVariable(baseKey+"12XYAngle",-90.0+84.0);
      Control.addVariable(baseKey+"13XYAngle",-90.0+78.0);
      Control.addVariable(baseKey+"14XYAngle",-90.0+72.0);
      Control.addVariable(baseKey+"15XYAngle",-90.0+66.0);
      Control.addVariable(baseKey+"16XYAngle",-90.0+60.0);
      Control.addVariable(baseKey+"17XYAngle",-90.0+54.0);
      Control.addVariable(baseKey+"18XYAngle",-90.0+48.0);
      Control.addVariable(baseKey+"19XYAngle",-90.0+42.0);
      Control.addVariable(baseKey+"20XYAngle",-90.0+36.0);
      Control.addVariable(baseKey+"21XYAngle",-90.0+30.0);  // W1
      Control.addVariable(baseKey+"Filled",0);
      Control.addVariable(baseKey+"Active",0);


    }
  return;
}

void
EssFlightLineVariables(FuncDataBase& Control)
  /*!
    Set the flightline variables
    \param Control :: Database for variables
   */
{
  ELog::RegMethod RegA("essVariables[F]","EssFlightLineVariables");

  // upper flight lines
  
  Control.addVariable("TopAFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopAFlightZStep", 0.0);      // Step from centre

  Control.addVariable("TopAFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("TopAFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("TopAFlightXYAngle",180.0);
  Control.addVariable("TopAFlightZAngle",0.0);
  // Step down angle !!! 1.455 is too much -
  Control.addVariable("TopAFlightAngleZTop",1.1);       // fight line cuts the Bilbao target wheel ESS-0032315.3
  Control.addVariable("TopAFlightAngleZBase",1.33);   // Step up angle ESS-0032315.3 

  // Full height = TopFlyTotalHeight
  Control.addVariable("TopAFlightHeight", 2.9);     
  Control.addVariable("TopAFlightWidth", 10.7);     // Full width
  Control.addVariable("TopAFlightNLiner",1);      
  Control.addVariable("TopAFlightLinerThick1",0.3);      
  Control.addVariable("TopAFlightLinerMat1","Aluminium");      

  // LOWER flight lines
  // B FLIGHT CORRECTED
  Control.addVariable("TopBFlightXStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightZStep", 0.0);      // Step from centre
  Control.addVariable("TopBFlightAngleXY1", 60.0);  // Angle out
  Control.addVariable("TopBFlightAngleXY2", 60.0);  // Angle out
  Control.addVariable("TopBFlightXYAngle", 0.0);
  Control.addVariable("TopBFlightZAngle", 0.0);
  Control.addParse<double>("TopBFlightAngleZTop","TopAFlightAngleZTop");
  Control.addParse<double>("TopBFlightAngleZBase","TopAFlightAngleZBase");
  Control.addParse<double>("TopBFlightHeight","TopAFlightHeight");
  Control.addParse<double>("TopBFlightWidth","TopAFlightWidth");
  Control.addParse<double>("TopBFlightLinerThick1","TopAFlightLinerThick1");
  Control.addVariable("TopBFlightNLiner",1);     
  Control.addVariable("TopBFlightLinerMat1","Aluminium");

  
  // The moderator focal points are (89,54)
  // The wedge focus at (80,48). This is a result of the line of sight from
  // the neutron beam channels towards the moderator
  const double wedgeFocusX = 8.0; // email from Rickard Holmberg 15 Sep 2016
  const double wedgeFocusY = 4.8; // email from Rickard Holmberg 15 Sep 2016

  const int TopAFlightNWedges = 0;
  Control.addVariable("TopAFlightNWedges",TopAFlightNWedges);

  const double t1 = 3.5+11.95*3.0; // Rickard Holmberg: slides 9-10
  const double dt1 = 11.95; // Rickard Holmberg ESS-0037906, ESS-0038057 + slide 7
  const double t2 = -9.1;   // Rickard Holmberg slide 12
  const double dt2 = 5.975; // Rickard Holmberg ESS-0037906, ESS-0038057
  
  std::vector<double> TopAFlightWedgeTheta;
  TopAFlightWedgeTheta.push_back(t1);
  for (size_t i=1; i<=3; i++)
    TopAFlightWedgeTheta.push_back(t1-dt1*static_cast<double>(i));

  // central wedge: Rickard Holmberg slide 14
  TopAFlightWedgeTheta.push_back(-2.8);
  TopAFlightWedgeTheta.push_back(t2);
  for (size_t i=1; i<=8; i++)
    TopAFlightWedgeTheta.push_back(t2-dt2*static_cast<double>(i));

  double xstep(0);
  double ystep(0);
  
  for (size_t i=1;i<=TopAFlightNWedges;i++)
    {
      const std::string baseKey =
        StrFunc::makeString("TopAFlightWedge", i);
      
      if (i==5) // central, the thick one
	{
	  Control.addVariable(baseKey+"BaseWidth", 12.0+2*30*std::tan(4*M_PI/180));  // Rickard Holmberg slide 14
	  Control.addVariable(baseKey+"TipWidth",  12.0); // Rickard Holmberg slide 14

	  Control.addVariable(baseKey+"XStep", 0.0);
	  Control.addVariable(baseKey+"YStep", 0.0);
	}
      else
	{
	  Control.addVariable(baseKey+"BaseWidth", 4.446+0.5*2); // Naja
	  Control.addVariable(baseKey+"TipWidth",  1.407+0.5*2); // Naja

	  xstep = wedgeFocusX;
	  ystep = (i<=4) ? wedgeFocusY : -wedgeFocusY;
	  Control.addVariable(baseKey+"XStep", xstep);
	  Control.addVariable(baseKey+"YStep", ystep);
	}
      Control.addVariable(baseKey+"ZStep", 13.7);

      Control.addVariable(baseKey+"Theta", TopAFlightWedgeTheta[i-1]);

      Control.addVariable(baseKey+"Length",30.0); // Naja
      Control.addVariable(baseKey+"Mat","SS316L");
    }


  const int TopBFlightNWedges = 0;
  Control.addVariable("TopBFlightNWedges",TopBFlightNWedges);
  std::vector<double> TopBFlightWedgeTheta;

  // email from Rickard Holmberg 15 Sep, slide 5
  const double t3 = (9.1+dt2*8.0)-180.0; 
  TopBFlightWedgeTheta.push_back(t3);
  for (size_t i=1;i<=8;i++)
    TopBFlightWedgeTheta.push_back(t3-dt2*static_cast<double>(i));

  // email from Rickard Holmberg 15 Sep, slide 6
  const double t4 = -15.45-180.0; 
  TopBFlightWedgeTheta.push_back(t4);

  TopBFlightWedgeTheta.push_back(t4-dt1);
  TopBFlightWedgeTheta.push_back(t4-2.0*dt1);

  if (TopBFlightNWedges > TopBFlightWedgeTheta.size())
    throw ColErr::RangeError<int>(TopBFlightNWedges,0,
				  static_cast<int>(TopBFlightWedgeTheta.size()),
				  "NWedges should not exceed size of TopBFlightWedgeTheta");
  
  for (size_t i=1; i<=TopBFlightNWedges; i++)
    {
      const std::string baseKey = StrFunc::makeString("TopBFlightWedge", i);

      Control.addVariable(baseKey+"BaseWidth",4.446+0.5*2); // Naja
      Control.addVariable(baseKey+"TipWidth", 1.407+0.5*2); // Naja

      const double xstep = -wedgeFocusX;
      const double ystep = (i<=9) ? -wedgeFocusY : wedgeFocusY;
      Control.addVariable(baseKey+"XStep", xstep);
      Control.addVariable(baseKey+"YStep", ystep);
      Control.addVariable(baseKey+"ZStep", 13.7);

      Control.addVariable(baseKey+"Theta", TopBFlightWedgeTheta[i-1]);

      Control.addVariable(baseKey+"Length", 30.0); // Naja
      Control.addVariable(baseKey+"Mat","SS316L");
    }


  // lower flight lines

  Control.addVariable("LowAFlightXStep",0.0); // Step from centre
  Control.addVariable("LowAFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowAFlightXYAngle",0.0);  // Angle out
  Control.addVariable("LowAFlightZAngle",0.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("LowAFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("LowAFlightAngleZTop",0.9);  // Step down angle ESS-0032315.3
  Control.addVariable("LowAFlightAngleZBase",0.9); // Step up angle ESS-0032315.3
  Control.addVariable("LowAFlightHeight",6.1);     // Full height =
  Control.addVariable("LowAFlightWidth",10.7);     // Full width
  Control.addVariable("LowAFlightNLiner",1);      // Liner
  Control.addVariable("LowAFlightLinerThick1",0.4);      // Liner
  Control.addVariable("LowAFlightLinerMat1","Aluminium");      // Liner
  Control.addVariable("LowAFlightLinerThick2",1.5);      // Liner
  Control.addVariable("LowAFlightLinerMat2","H2O");      // Liner
  Control.addVariable("LowAFlightLinerThick3",0.2);      // Liner
  Control.addVariable("LowAFlightLinerMat3","Aluminium");      // Liner
  Control.addVariable("LowAFlightLinerThick4",0.3);      // Liner
  Control.addVariable("LowAFlightLinerMat4","Void");      // Liner

  Control.addVariable("LowBFlightTapSurf","cone");  // Step down angle
  Control.addVariable("LowBFlightXStep",0.0);     // Angle
  Control.addVariable("LowBFlightZStep",0.0);      // Step from centre
  Control.addVariable("LowBFlightXYAngle",180.0);  // Angle out
  Control.addVariable("LowBFlightZAngle",0.0);     // Angle out
  Control.addVariable("LowBFlightAngleXY1",60.0);  // Angle out
  Control.addVariable("LowBFlightAngleXY2",60.0);  // Angle out
  Control.addVariable("LowBFlightAngleZTop",0.9);  // Step down angle
  Control.addVariable("LowBFlightAngleZBase",0.9); // Step up angle
  Control.addVariable("LowBFlightHeight",6.1);     // Full height 
  Control.addVariable("LowBFlightWidth",10.7);     // Full width
  Control.addVariable("LowBFlightNLiner",1);      // Liner
  Control.addVariable("LowBFlightLinerThick1",0.4);   
  Control.addVariable("LowBFlightLinerMat1","Aluminium");      


  return;
}


void
EssInstrumentVariables(const std::set<std::string>& BL,
                       FuncDataBase& Control)
  /*!
    Construct the variables for the beamlines if required
    \param BL :: Set for the beamlines
    \param Control :: Database for variables
   */
{
  ELog::RegMethod RegA("essVariables[F]",
                       "EssInstrumentVariables");

  typedef void (*VariableFunction)(FuncDataBase&);
  typedef std::multimap<std::string,VariableFunction> VMap;
  
  const VMap VarInit({
     {"BEER",        &BEERvariables},
     {"BIFROST",     &BIFROSTvariables},
     {"CSPEC",       &CSPECvariables},
     {"DREAM",       &DREAMvariables},     
     {"ESTIA",       &ESTIAvariables},   
     {"FREIA",       &FREIAvariables},
     {"HEIMDAL",     &HEIMDALvariables},
     {"LOKI",        &LOKIvariables},
     {"MAGIC",       &MAGICvariables},
     {"MIRACLES",    &MIRACLESvariables},
     {"NMX",         &NMXvariables},
     {"NNBAR",       &NNBARvariables},
     {"ODIN",        &ODINvariables},
     {"SKADI",       &SKADIvariables},
     {"TREX",        &TREXvariables},
     {"TESTBEAM",    &TESTBEAMvariables},
     {"VESPA",       &VESPAvariables},
     {"VOR",         &VORvariables},
     {"SHORTNMX",     &NMXvariables},
     {"SHORTNMX",     &shortNMXvariables},
     {"SHORTDREAM",   &DREAMvariables},
     {"SHORTDREAM",   &shortDREAMvariables},
     {"SHORTDREAM2",  &DREAMvariables},
     {"SHORTDREAM2",  &shortDREAM2variables},
     {"SHORTODIN",    &ODINvariables},
     {"SHORTODIN",    &shortODINvariables}

       
   });

  
  for(const std::string& beam : BL)
    {
      // std::pair<VMap::const_iterator,VMap::const_iterator>
      VMap::const_iterator mc;
      decltype(VarInit.equal_range("")) rangePair
	= VarInit.equal_range(beam);
      for(mc=rangePair.first;mc!=rangePair.second;mc++)
	mc->second(Control);
    }
  
  simpleITEMvariables(Control);

  return;
}  
  
}  // NAMESPACE setVariable
