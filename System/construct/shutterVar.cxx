/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/shutterVar.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <boost/format.hpp>

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
#include "shutterVar.h"

namespace ts1System
{

shutterVar::shutterVar(const std::string& K) :
  keyName(K),NSections(8),steelNumber(4),colletHGap(0.6),
  colletVGap(0.6),colletFGap(0.6),colletMat("CastIron"),
  b4cThick(0.635),b4cSpace(0.2),steelSize(NSections+1),
  blockVertGap(1.0),blockHorGap(1.0)
  /*!
    Constructor
    \param K :: Key name
  */
{
  const size_t ASection(4);
  for(size_t i=0;i<ASection;i++)
    steelSize[i]=std::pair<double,double>(8.075,7.075);
  for(size_t i=ASection;i<=NSections;i++)
    steelSize[i]=std::pair<double,double>(10.50,8.1);    
}

shutterVar::shutterVar(const shutterVar& A) : 
  keyName(A.keyName),NSections(A.NSections),
  steelNumber(A.steelNumber),
  colletHGap(A.colletHGap),colletVGap(A.colletVGap),
  colletFGap(A.colletFGap),colletMat(A.colletMat),
  b4cThick(A.b4cThick),b4cSpace(A.b4cSpace),
  steelSize(A.steelSize),blockVertGap(A.blockVertGap),
  blockHorGap(A.blockHorGap)
  /*!
    Copy constructor
    \param A :: shutterVar to copy
  */
{}

shutterVar&
shutterVar::operator=(const shutterVar& A)
  /*!
    Assignment operator
    \param A :: shutterVar to copy
    \return *this
  */
{
  if (this!=&A)
    {
      keyName=A.keyName;
      NSections=A.NSections;
      steelNumber=A.steelNumber;
      colletHGap=A.colletHGap;
      colletVGap=A.colletVGap;
      colletFGap=A.colletFGap;
      colletMat=A.colletMat;
      b4cThick=A.b4cThick;
      b4cSpace=A.b4cSpace;
      steelSize=A.steelSize;
      blockVertGap=A.blockVertGap;
      blockHorGap=A.blockHorGap;
    }
  return *this;
}

void
shutterVar::setSteelOffsets(const size_t AIndex,const size_t BIndex,
			    const double W,const double H)
  /*!
    Sets the default range of the shutter steel section 
    \param AIndex :: Initial index
    \param BIndex :: Final index
    \param W :: Width
    \param H :: Height
   */
{
  for(size_t i=AIndex;i<BIndex && i<=NSections;i++)
    steelSize[i]=std::pair<double,double>(W,H);
  return;
}


double
shutterVar::SteelBlock(FuncDataBase& Control,const size_t index,const double CX,
		       const double Len,const double HSize,const double VSize,
		       const double VG,const double HG,const std::string& mat,
		       const size_t extra) const
  /*!
    Construct a steel block for the Zoom beamline
    \param Control :: Database item
    \param index :: Index nubmer 
    \param CX :: Centre offset distance
    \param Len :: Length of steel block
    \param HSize :: Height/Width
    \param VSize :: Height 
    \param VG :: Vertical gap
    \param HG :: Horrizontal gap
    \param mat :: Material name
    \param extra :: Number of blocks
    \return extension length
   */
{
  boost::format FMT(keyName+"ShutterBlock%d%s");
  Control.addVariable((FMT % index % "CentX").str(),CX);
  Control.addVariable((FMT % index % "Len").str(),Len);
  Control.addVariable((FMT % index % "Width").str(),HSize);
  Control.addVariable((FMT % index % "Height").str(),VSize);
  Control.addVariable((FMT % index % "VGap").str(),VG);
  Control.addVariable((FMT % index % "HGap").str(),HG);
  Control.addVariable((FMT % index % "Mat").str(),mat);
  for(size_t i=0;i<extra;i++)
    Control.addVariable((FMT % (index+i) % "Mat").str(),mat);

  return Len*static_cast<double>(extra);
}


double
shutterVar::B4Cspacer(FuncDataBase& Control,const size_t index,
		      const double CX,const double CZ,
		      const double GX,const double GZ) const
/*!
    Function to do the populate of the shutter blocks [SQUARE]
    \param Control :: Func database
    \param index :: index
    \param CX :: Displacement in CX
    \param CZ :: Displacement in CZ
    \param GX :: Gap in X
    \param GZ :: Gap in Z
    \return Length of this component
  */
{
  boost::format FMT(keyName+"ShutterBlock%d%s");
  const std::string b4cMat("B4C");

  // B4C scrapper #1 
  Control.addVariable((FMT % index % "CentX").str(),CX);
  Control.addVariable((FMT % index % "CentZ").str(),CZ);
  Control.addVariable((FMT % index % "Len").str(),b4cThick);
  Control.addVariable((FMT % index % "VGap").str(),GZ+1.0);
  Control.addVariable((FMT % index % "HGap").str(),GX);
  Control.addVariable((FMT % index % "Mat").str(),b4cMat);
  Control.addVariable((FMT % (index+1) % "Mat").str(),"Void");
  Control.addVariable((FMT % (index+1) % "Len").str(),b4cSpace);
  Control.addVariable((FMT % (index+2) % "Mat").str(),b4cMat);
  Control.addVariable((FMT % (index+2) % "Len").str(),b4cThick);
  Control.addVariable((FMT % (index+2) % "VGap").str(),GZ);
  Control.addVariable((FMT % (index+2) % "HGap").str(),GX+1.0);

  return 2.0*b4cThick+b4cSpace;
}

double
shutterVar::B4Cspacer(FuncDataBase& Control,const size_t index,
		      const double CX,const double CZ,
		      const double GR) const
/*!
    Function to do the populate of the shutter blocks [CIRCLE]
    \param Control :: Func database
    \param index :: index
    \param CX :: Displacement in CX
    \param CZ :: Displacement in CZ
    \param GR :: Gap in Radius
    \return Length of this component
  */
{
  boost::format FMT(keyName+"ShutterBlock%d%s");
  const std::string b4cMat("B4C");
  double gapThick((b4cSpace+b4cThick)/2.0);     // normal is two b4c sheets this is one

  // B4C scrapper #1 

  Control.addVariable((FMT % index % "TYPE").str(),1);
  Control.addVariable((FMT % index % "CentX").str(),CX);
  Control.addVariable((FMT % index % "CentZ").str(),CZ);
  Control.addVariable((FMT % index % "Len").str(),gapThick);
  Control.addVariable((FMT % index % "RGap").str(),GR+1.0);
  Control.addVariable((FMT % index % "Mat").str(),"Void");

  Control.addVariable((FMT % (index+1) % "TYPE").str(),1);
  Control.addVariable((FMT % (index+1) % "Mat").str(),b4cMat);
  Control.addVariable((FMT % (index+1) % "Len").str(),b4cThick);
  Control.addVariable((FMT % (index+1) % "RGap").str(),GR);

  Control.addVariable((FMT % (index+2) % "TYPE").str(),1);
  Control.addVariable((FMT % (index+2) % "Mat").str(),"Void");
  Control.addVariable((FMT % (index+2) % "Len").str(),gapThick);
  Control.addVariable((FMT % (index+2) % "RGap").str(),GR+1.0);

  return 2.0*b4cThick+b4cSpace;
}

void
shutterVar::header(FuncDataBase& Control,const double xAngle,
		   const double xStep,const double zAngle) const
  /*!
    Build the header
    \param Control :: Control values
    \param xStep :: X position of the beam axis [start]
    \param xAngle :: X angle of the beam axis
    \param zAngle :: Z angle of the beam axis
  */
{
  // SHUTTER BLOCK  
  Control.addVariable(keyName+"ShutterColletHGap",colletHGap);
  Control.addVariable(keyName+"ShutterColletVGap",colletVGap);
  Control.addVariable(keyName+"ShutterColletFGap",colletFGap);
  Control.addVariable(keyName+"ShutterColletMat","CastIron");  //   surround material  

  Control.addVariable(keyName+"ShutterXStart",0.0);           // Start Z position
  Control.addVariable(keyName+"ShutterZStart",0.0);           // Start Z position
  Control.addVariable(keyName+"ShutterGuideXAngle",xAngle);   // rotation off center
  Control.addVariable(keyName+"ShutterGuideXStep",xStep);     // rotation off center
  Control.addVariable(keyName+"ShutterGuideZAngle",zAngle);   // drop in mRad

  Control.addVariable(keyName+"ShutterBlock1FStep",0.0);    //   Inner blocks
  Control.addVariable(keyName+"ShutterBlock1CentX",0.0);    //   Inner blocks
  Control.addVariable(keyName+"ShutterBlock1CentZ",0.0);    //   Inner blocks
  Control.addVariable(keyName+"ShutterBlock1Len",3.0);      //   Inner blocks
  Control.addVariable(keyName+"ShutterBlock1Width",steelSize[0].first); 
  Control.addVariable(keyName+"ShutterBlock1Height",steelSize[0].second); 
  Control.addVariable(keyName+"ShutterBlock1VGap",3.0);     //   Height
  Control.addVariable(keyName+"ShutterBlock1HGap",3.0);     //   Hor. Gap (full)
  Control.addVariable(keyName+"ShutterBlock1Mat","Void");        //   Spacer
  return;
}

void
shutterVar::buildVar(FuncDataBase& Control,
		     const double xStep,const double xAngle,
		     const double zAngle,const double beamX,
		     const double beamZ,const double angleX,
		     const double angleZ) const
  /*!
    ShutterVariable for a give shutter [No Tapor]
    \param Control :: FuncDatabase 
    \param xStep :: X position of the beam axis [start]
    \param xAngle :: X angle of the beam axis
    \param zAngle :: Z angle of the beam axis
    \param beamX :: Beam size X [b4c]
    \param beamZ :: Beam size Z [b4c]
    \param angleX :: Beam angle drop X [b4c]
    \param angleZ :: Beam angle drop Z [b4c]
  */
{
  ELog::RegMethod RegA("shutterVar","buildVar");
  header(Control,xStep,xAngle,zAngle);

  const size_t b4cNumber(3);
  size_t blockIndex(2); 
  const std::string steelMat("Stainless304");
  const double shutterInnerLen(179.2);

  const double steelLen(shutterInnerLen/
			static_cast<double>(NSections*steelNumber));

  double pathLen(0.0);
  for(size_t i=0;i<=NSections;i++)
    {
      const double steelWidth(steelSize[i].first);
      const double steelHeight(steelSize[i].second);

      const double bX=beamX-pathLen*tan(angleX*M_PI/180.0);
      const double bZ=beamZ-pathLen*tan(angleZ*M_PI/180.0);

      pathLen+=B4Cspacer(Control,blockIndex,0.0,0.0,bX,bZ);
      if (i!=NSections)
	{
	  pathLen+=SteelBlock(Control,blockIndex+b4cNumber,0.0,
			      steelLen,steelWidth,steelHeight,
			      blockVertGap+bZ,blockHorGap+bX,
			      steelMat,steelNumber);
	  blockIndex+=static_cast<size_t>(steelNumber)+b4cNumber;
	}
    }


  // Last B4C scrapper addition
  blockIndex+=b4cNumber;
  const std::string finalBlock=
    StrFunc::makeString(std::string(keyName+"ShutterBlock"),blockIndex);
  Control.addVariable(keyName+"ShutterNBlocks",blockIndex);     //   Inner blocks

  Control.addVariable(finalBlock+"CentX",0.0);    //   Inner blocks
  Control.addVariable(finalBlock+"CentZ",0.0);    //   Inner blocks
  Control.addVariable(finalBlock+"Len",3.4);      //   Inner blocks
  Control.addVariable(finalBlock+"VGap",4.0);     //   Height
  Control.addVariable(finalBlock+"HGap",4.0);     //   Hor. Gap (full)
  Control.addVariable(finalBlock+"Mat","Void");   //   Spacer

  return;
}

void
shutterVar::buildCylVar(FuncDataBase& Control,
			const double xStep,const double xAngle,
			const double zAngle,const double beamDiam,
			const double angleR) const
  /*!
    ShutterVariable for a give shutter [No Tapor]
    \param Control :: FuncDatabase 
    \param xStep :: Step across beam 
    \param xAngle :: Rotation across beam 
    \param zAngle :: Rotation up/down beam 
    \param beamDiam :: Beam Diameter [b4c]
    \param angleR :: Beam angle drop of size [b4c]
  */
{
  ELog::RegMethod RegA("shutterVar","buildClyVar");
  // SHUTTER BLOCK  
  Control.addVariable(keyName+"ShutterColletHGap",colletHGap);
  Control.addVariable(keyName+"ShutterColletVGap",colletVGap);
  Control.addVariable(keyName+"ShutterColletFGap",colletFGap);
  Control.addVariable(keyName+"ShutterColletMat","CastIron"); // surround mat

  Control.addVariable(keyName+"ShutterXStart",0.0);           // Start Z position
  Control.addVariable(keyName+"ShutterZStart",0.0);           // Start Z position
  Control.addVariable(keyName+"ShutterGuideXAngle",xAngle);   // rotation off center
  Control.addVariable(keyName+"ShutterGuideXStep",xStep);     // rotation off center
  Control.addVariable(keyName+"ShutterGuideZAngle",zAngle);   // drop in mRad

  const int b4cNumber(3);

  Control.addVariable(keyName+"ShutterBlock1FStep",0.0);   
  Control.addVariable(keyName+"ShutterBlock1CentX",0.0);   
  Control.addVariable(keyName+"ShutterBlock1CentZ",0.0);   
  Control.addVariable(keyName+"ShutterBlock1Len",3.0);     
  Control.addVariable(keyName+"ShutterBlock1Width",steelSize[0].first); 
  Control.addVariable(keyName+"ShutterBlock1Height",steelSize[0].second); 
  Control.addVariable(keyName+"ShutterBlock1VGap",3.0);     // Height
  Control.addVariable(keyName+"ShutterBlock1HGap",3.0);     // Hor. Gap (full)
  Control.addVariable(keyName+"ShutterBlock1Mat","Void");   //   Spacer

  size_t blockIndex(2); 
  const std::string steelMat("Stainless304");

  const double steelLen(179.2/
			static_cast<double>(NSections*steelNumber));

  double pathLen(0.0);
  for(size_t i=0;i<=NSections;i++)
    {
      const double steelWidth(steelSize[i].first);
      const double steelHeight(steelSize[i].second);

      const double bR=(beamDiam-pathLen*tan(angleR*M_PI/180.0))/2.0;
      pathLen+=B4Cspacer(Control,blockIndex,0.0,0.0,bR);
      blockIndex+=b4cNumber;
      if (i!=NSections)
	{
	  pathLen+=SteelBlock(Control,blockIndex,0.0,
			      steelLen,steelWidth,steelHeight,
			      blockVertGap+2*bR,blockHorGap+2*bR,
			      steelMat,steelNumber);
	  blockIndex+=steelNumber;
	}
    } 
  const std::string finalBlock=
    StrFunc::makeString(std::string(keyName+"ShutterBlock"),blockIndex);
  Control.addVariable(keyName+"ShutterNBlocks",blockIndex);     //   Inner blocks 

  Control.addVariable(finalBlock+"CentX",0.0);  //   Inner blocks
  Control.addVariable(finalBlock+"CentZ",0.0);  //   Inner blocks
  Control.addVariable(finalBlock+"Len",3.4);    //   Inner blocks
  Control.addVariable(finalBlock+"VGap",4.0);   //   Height
  Control.addVariable(finalBlock+"HGap",4.0);   //   Hor. Gap (full)
  Control.addVariable(finalBlock+"Mat","Void");      //   Spacer

  return;
}

}   // NAMESAPCE 
