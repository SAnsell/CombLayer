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
#include <boost/shared_ptr.hpp>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "F5Calc.h"
#include "F5Collimator.h"

namespace essSystem
{

  F5Collimator::F5Collimator(const std::string& Key) :
    attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
    colIndex(ModelSupport::objectRegister::Instance().cell(Key)),
    cellIndex(colIndex+1)
    /*!
      Constructor
      \param Key :: Name of construction key
    */
  {
    theta = -1111;
  }

  F5Collimator::F5Collimator(const F5Collimator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
    tallySystem::F5Calc(A),
    colIndex(A.colIndex), cellIndex(A.cellIndex),
    delta(A.delta),height(A.height),length(A.length),width(A.width),
    wall(A.wall),viewWidth(A.viewWidth),LinkPoint(A.LinkPoint),
    radius(A.radius),theta(A.theta),vecFP(A.vecFP),range(A.range),
    lpAlgorithm(A.lpAlgorithm)
  {
  }

  F5Collimator&
  F5Collimator::operator=(const F5Collimator& A)
  {
    if (this!=&A)
      {
	attachSystem::ContainedComp::operator=(A);
	attachSystem::FixedComp::operator=(A);
	tallySystem::F5Calc::operator=(A);
	cellIndex=A.cellIndex;
	delta=A.delta;
	height=A.height;
	length=A.length;
	width=A.width;
	wall=A.wall;
	viewWidth=A.viewWidth;
	LinkPoint=A.LinkPoint;
	radius=A.radius;
	theta=A.theta;
	vecFP=A.vecFP;
	range=A.range;
	lpAlgorithm=A.lpAlgorithm;
      }
    return *this;
  }

  F5Collimator*
  F5Collimator::clone() const
  {
    return new F5Collimator(*this);
  }

  F5Collimator::~F5Collimator()
  /*!
    Destructor
  */
  {}

  void
  F5Collimator::populate(FuncDataBase& Control)
  /*!
    Populate all the variables for collimators without link points
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("F5Collimator","populate");

    xStep=Control.EvalVar<double>(keyName+"X");
    yStep=Control.EvalVar<double>(keyName+"Y");
    zStep=Control.EvalVar<double>(keyName+"Z");

    length=Control.EvalVar<double>(keyName+"Length"); // along x
    wall=Control.EvalDefVar<double>(keyName+"WallThick", 0.5);

    LinkPoint = Control.EvalDefVar<int>(keyName+"LinkPoint", -1);

    Geometry::Vec3D gC,gB,gB2;

    gB[0]=Control.EvalDefVar<double>(keyName+"XB", 0);
    gB[1]=Control.EvalDefVar<double>(keyName+"YB", 0);
    gB[2]=Control.EvalDefVar<double>(keyName+"ZB", 0);

    gC[0]=Control.EvalDefVar<double>(keyName+"XC", 0);
    gC[1]=Control.EvalDefVar<double>(keyName+"YC", 0);
    gC[2]=Control.EvalDefVar<double>(keyName+"ZC", 0);

    gB2[2] = Control.EvalDefVar<double>(keyName+"ZG", 0);
    

    gB2[0] = gB[0];
    gB2[1] = gB[1];

    SetTally(xStep, yStep, zStep);
    SetPoints(gB, gC, gB2);
    SetLength(length);
    xyAngle = GetXYAngle();
    zAngle  = GetZAngle();
    width = 2*GetHalfSizeX();
    height = 2*GetHalfSizeZ();

    return;
  }


  void F5Collimator::populateWithTheta(FuncDataBase& Control)
  /*!
    Populate all the variables in the case when \theta is defined
    (i.e. if theta is defined then link points are calculated automatically)
    \param Control :: Variable table to use
  */
  {
    ELog::RegMethod RegA("F5Collimator","populateWithTheta");

    if (theta<0) 
      {
	ELog::EM << "Theta must be defined when this method is used. Call F5Collimator::populate() if link points are not needed"
		 << ELog::endErr;
	return;
      }

    // check the range value:
    if ((range.find("cold")==std::string::npos) && (range.find("thermal")==std::string::npos))
      throw ColErr::InvalidLine(range,"range can be either \"cold\" or \"thermal\"");

    radius=Control.EvalDefPair<double>(keyName, "F5Default", "Radius", -1);  // Must be positive to be used with theta.
    if (radius<=0)
	throw ColErr::RangeError<double>(radius, 0, INFINITY, "Radius must be positive if used with theta");

    length=Control.EvalPair<double>(keyName, "F5Default", "Length"); // along x
    wall=Control.EvalDefVar<double>(keyName+"WallThick", 0.5);
    viewWidth=Control.EvalPair<double>(keyName, "F5Default", "ViewWidth");
    delta = Control.EvalDefVar<double>(keyName+"Delta", 0.0);
    lpAlgorithm = Control.EvalPair<std::string>(keyName, "F5Default", "Algorithm"); // "FocalPoint" "InnerFocalPoint" "MidWaterEdge" "MidWaterSide" "manual"

    // xyz coordinates of F5 tally
    Control.setVariable<double>(keyName+"X", radius*sin(theta*M_PI/180.0));
    Control.setVariable<double>(keyName+"Y", radius*cos(theta*M_PI/180.0));

    xStep=Control.EvalVar<double>(keyName+"X");
    yStep=Control.EvalVar<double>(keyName+"Y");

    if (vecFP.size()<2)
      throw ColErr::RangeError<double>(vecFP.size(), 2, INFINITY,
				       "Focal point vector must have at least 2 entries with zmin/zmax.");

    const double zmin = vecFP[vecFP.size()-2].Z();
    const double zmax = vecFP[vecFP.size()-1].Z();
    Control.setVariable<double>(keyName+"Z", (zmin+zmax)/2.0);
    zStep=Control.EvalVar<double>(keyName+"Z");

    Geometry::Vec3D gC,gB,gB2;

    int lp=0;
    // link point (defined by theta)
    if (lpAlgorithm == "FocalPoint")
      {
	if (theta<90)
	  lp =  zStep>0 ? 6 : 4; // OK these maths depend on the XYangle of the moderator
	else if (theta<180)
	  lp =  zStep>0 ? 7 : 5;
	else if (theta<270)
	  lp =  zStep>0 ? 5 : 7; // OK
	else // if theta>270
	  lp =  zStep>0 ? 4 : 6; // OK
      } else if (lpAlgorithm == "InnerFocalPoint")
      {
	if (theta<90)
	  lp =  zStep>0 ? 10 : 8; // OK these maths depend on the XYangle of the moderator
	else if (theta<180)
	  lp =  zStep>0 ? 11 : 9;
	else if (theta<270)
	  lp =  zStep>0 ? 9 : 11;
	else // if theta>270
	  lp =  zStep>0 ? 8 : 10;
      } else if (lpAlgorithm == "MidWaterEdge")
      {
	if (theta<90)
	  lp =  zStep>0 ? 14 : 12; // OK these maths depend on the XYangle of the moderator
	else if (theta<180)
	  lp =  zStep>0 ? 15 : 13;
	else if (theta<270)
	  lp =  zStep>0 ? 13 : 15;
	else // if theta>270
	  lp =  zStep>0 ? 12 : 14;
      } else if (lpAlgorithm == "MidWaterSide")
      {
	if (theta<90)
	  lp =  zStep>0 ? 18 : 16; // OK these maths depend on the XYangle of the moderator
	else if (theta<180)
	  lp =  zStep>0 ? 19 : 17;
	else if (theta<270)
	  lp =  zStep>0 ? 17 : 19;
	else // if theta>270
	  lp =  zStep>0 ? 16 : 18;
      } else if (lpAlgorithm == "manual")
      {
	// "special" link points - needed to define the BOC sign
	if (theta<90)
	  lp =  zStep>0 ? -3 : -1;
	else if (theta<180)
	  lp =  zStep>0 ? -4 : -2;
	else if (theta<270)
	  lp =  zStep>0 ? -2 : -4;
	else // if theta>270
	  lp =  zStep>0 ? -1 : -3;
      } else
      throw ColErr::InvalidLine(lpAlgorithm,"Link point algorithm not in 'FocalPoint', 'InnerFocalPoint', 'MidWaterEdge', 'MidWaterSide' or 'manual'");
    
    Control.setVariable<int>(keyName+"LinkPoint", lp);
    LinkPoint = Control.EvalVar<int>(keyName+"LinkPoint");


    // Calculate the coordinate of L (the second point)
    /*
      F and L are two points where the collimator looks
      O is the F5 tally location

      C
      |
      |                              O(xStep, yStep, zStep)
      |
      B(LinkPoint+0, LinkPoint+1, zStep)

    */

    Geometry::Vec3D B;
    if (LinkPoint < 0)
      {
	Geometry::Vec3D tmpB = Control.EvalVar<Geometry::Vec3D>(keyName+"FocalPoint");
	B = Geometry::Vec3D(tmpB.X(), tmpB.Y(), zmax);
      } else
      {
	B = Geometry::Vec3D(vecFP[static_cast<unsigned int>(LinkPoint)].X(),
			    vecFP[static_cast<unsigned int>(LinkPoint)].Y(), zmax);
      }
    
    Geometry::Vec3D OB(B[0]-xStep, B[1]-yStep, B[2]-zStep);

    // Calculate angle BOC by the law of cosines:
    double BOC = std::acos((2*std::pow(OB.abs(), 2) - std::pow(viewWidth, 2))/(2*std::pow(OB.abs(), 2)));
    // define direction of C with respect to B:
    //  (these maths depend on the XYangle of the moderator)
    if (zStep>0) {// top moderator
      if ((LinkPoint==-2) || (LinkPoint==-3) || (LinkPoint==5) || (LinkPoint==6) || (LinkPoint==9) || (LinkPoint==10) || (LinkPoint==13) || (LinkPoint==14) || (LinkPoint==17) || (LinkPoint==18))
	BOC *= -1;
    } else { // low moderator
      if ((LinkPoint==-1) || (LinkPoint==-4) || (LinkPoint==4) || (LinkPoint==7) || (LinkPoint==8) || (LinkPoint==11) || (LinkPoint==12) || (LinkPoint==15) || (LinkPoint==16) || (LinkPoint==19))
	BOC *= -1;
    }

    // thermal collimators view complementary area:
    if (range=="thermal")
      BOC *= -1;

    Geometry::Vec3D OC(OB);
    Geometry::Quaternion::calcQRotDeg(BOC*180/M_PI,Z).rotate(OC);

    Geometry::Vec3D BC(OC-OB);
    if (BC.abs()-viewWidth>Geometry::zeroTol)
      ELog::EM << "Problem with tally " << keyName << ": distance between B and C is " << BC.abs() << " --- not equal to F5ViewWidth = " << viewWidth << ELog::endErr;

    Geometry::Vec3D C(B+BC);

    Control.setVariable<double>(keyName+"XB", B.X());
    Control.setVariable<double>(keyName+"YB", B.Y());
    Control.setVariable<double>(keyName+"ZB", B.Z());

    Control.setVariable<double>(keyName+"XC", C.X());
    Control.setVariable<double>(keyName+"YC", C.Y());
    Control.setVariable<double>(keyName+"ZC", C.Z());

    Control.setVariable<double>(keyName+"ZG", zmin);

    gB[0]=Control.EvalDefVar<double>(keyName+"XB", 0);
    gB[1]=Control.EvalDefVar<double>(keyName+"YB", 0);
    gB[2]=Control.EvalDefVar<double>(keyName+"ZB", 0);

    gC[0]=Control.EvalDefVar<double>(keyName+"XC", 0);
    gC[1]=Control.EvalDefVar<double>(keyName+"YC", 0);
    gC[2]=Control.EvalDefVar<double>(keyName+"ZC", 0);

    gB2[2] = Control.EvalDefVar<double>(keyName+"ZG", 0);

    gB2[0] = gB[0];
    gB2[1] = gB[1];

    SetTally(xStep, yStep, zStep);
    SetPoints(gB, gC, gB2);
    SetLength(length);
    xyAngle = GetXYAngle()+delta;
    zAngle  = GetZAngle();
    width = 2*GetHalfSizeX();
    height = 2*GetHalfSizeZ();

    return;
  }

  void F5Collimator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
  {
    ELog::RegMethod RegA("F5Collimator","createUnitVector");
    attachSystem::FixedComp::createUnitVector(FC);
    applyShift(xStep,yStep,zStep);
    applyAngleRotate(xyAngle,zAngle);
  
    return;
  }

  void F5Collimator::createSurfaces()
  {
    /*!
      Create Surfaces for the F5 collimator
    */
    ELog::RegMethod RegA("F5Collimator","createSurfaces");
      
    ModelSupport::buildPlane(SMap,colIndex+1, Origin-X*1.0, X);
    ModelSupport::buildPlane(SMap,colIndex+11, Origin-X*(1.0+wall), X);
    ModelSupport::buildPlane(SMap,colIndex+2, Origin+X*length, X);

    ModelSupport::buildPlane(SMap,colIndex+3, Origin-Y*(width/2.0), Y);
    ModelSupport::buildPlane(SMap,colIndex+13, Origin-Y*(width/2.0+wall), Y);
    ModelSupport::buildPlane(SMap,colIndex+4, Origin+Y*(width/2.0), Y);
    ModelSupport::buildPlane(SMap,colIndex+14, Origin+Y*(width/2.0+wall), Y);

    ModelSupport::buildPlane(SMap,colIndex+5, Origin-Z*(height/2.0), Z); 
    ModelSupport::buildPlane(SMap,colIndex+15, Origin-Z*(height/2.0+wall), Z);
    ModelSupport::buildPlane(SMap,colIndex+6, Origin+Z*(height/2.0), Z);
    ModelSupport::buildPlane(SMap,colIndex+16, Origin+Z*(height/2.0+wall), Z);

    return; 
  }

  void F5Collimator::setTheta(double t)
  {
    ELog::RegMethod RegA("F5Collimator","setTheta");
    
    if ((t<0) || (t>360))
      throw ColErr::RangeError<double>(theta, 0, 360, "Theta must be set in range 0-360 deg");
    theta=t;
  }

  void F5Collimator::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
  {
    ELog::RegMethod RegA("F5Collimator","addToInsertChain");
    
    for(int i=colIndex+1;i<cellIndex;i++)
      CC.addInsertCell(i);
    
    return;
  }


  void F5Collimator::createObjects(Simulation& System)
  /*!
    Create the F5 collimator
    \param System :: Simulation to add results
  */
  {
    ELog::RegMethod RegA("F5Collimator","createObjects");

    std::string Out;
  
    int voidMat = 0;//1001;
    Out=ModelSupport::getComposite(SMap,colIndex, " 11 -2 13 -14 15 -16");
    addOuterSurf(Out);

    // Internal region
    Out=ModelSupport::getComposite(SMap,colIndex," 1 -2 3 -4 5 -6");
    System.addCell(MonteCarlo::Qhull(cellIndex++, voidMat, 0.0, Out));

    // Wall
    Out=ModelSupport::getComposite(SMap, colIndex, " (11 -2 13 -14 15 -16) (-1:2:-3:4:-5:6) ");
    MonteCarlo::Qhull c = MonteCarlo::Qhull(cellIndex++, voidMat, 0.0, Out);
    c.setImp(0);
    System.addCell(c);

    return; 
  }

  void F5Collimator::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
  {
    ELog::RegMethod RegA("F5Collimator","createLinks");

    FixedComp::setConnect(0,   Origin-X*(1.0+wall), -X);   FixedComp::setLinkSurf(0, -SMap.realSurf(colIndex+1));
    FixedComp::setConnect(1,   Origin+X*length,  X);

    FixedComp::setConnect(2,   Origin-Y*(width/2+wall), -Y);
    FixedComp::setConnect(3,   Origin+Y*(width/2+wall),  Y);

    FixedComp::setConnect(4,   Origin-Z*(height/2+wall), -Z); 
    FixedComp::setConnect(5,   Origin+Z*(height/2+wall),  Z); 

    for (size_t i=0; i<6; i++)
      FixedComp::setLinkSurf(i, SMap.realSurf(colIndex+static_cast<int>(i)));
    
    return;
  }


  void
  F5Collimator::createAll(Simulation& System, const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
  */
  {
    ELog::RegMethod RegA("F5Collimator","createAll");
    if (theta>0)
      populateWithTheta(System.getDataBase()); // build collimators with link points
    else
      populate(System.getDataBase()); // no link points

    createUnitVector(FC);
    createSurfaces();
    createObjects(System);
    createLinks();
    insertObjects(System);       

    return;
  }

}  // namespace essSystem
