/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/pointTally.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "pointTally.h"

namespace tallySystem
{

pointTally::pointTally(const int ID) :
  Tally(ID),Radius(0.0),fuFlag(0),
  ddFlag(0,0,0),secondDFlag(0),secondDist(0.0),
  mcnp6Out(0)
  /*!
    Constructor
    \param ID :: Identity number of tally
  */
{}


pointTally::pointTally(const pointTally& A) : 
  Tally(A),Centre(A.Centre),Radius(A.Radius),
  fuFlag(A.fuFlag),
  fuCard(A.fuCard),ddFlag(A.ddFlag),Window(A.Window),
  secondDFlag(A.secondDFlag),secondDist(A.secondDist),
  mcnp6Out(A.mcnp6Out)
  /*!
    Copy constructor
    \param A :: pointTally to copy
  */
{}

pointTally&
pointTally::operator=(const pointTally& A)
  /*!
    Assignment operator
    \param A :: pointTally to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Tally::operator=(A);
      Centre=A.Centre;
      Radius=A.Radius;
      fuFlag=A.fuFlag;
      fuCard=A.fuCard;
      ddFlag=A.ddFlag;
      Window=A.Window;
      secondDFlag=A.secondDFlag;
      secondDist=A.secondDist;
      mcnp6Out=A.mcnp6Out;
    }
  return *this;
}

pointTally*
pointTally::clone() const
  /*!
    Clone object
    \return new (*this)
  */
{
  return new pointTally(*this);
}

pointTally::~pointTally()
  /*!
    Destructor
  */
{}


void
pointTally::setFUflag(const int flag)
  /*!
    Sets the fu (special treatment) flag
    \param flag :: flag value
  */
{
  fuFlag=flag;
  return;
}

void
pointTally::setDDFlag(const double D,const int outFlag) 
  /*!
    Sets the weighting system:
    - D<0 scores >|D| will be made and other subject to roulette
    - D>0 then for ddmp histories all, then only >Ak are made. 
    - D==0 the all are made.
    \param D :: weighting system
    \param outFlag :: Output type 0/none >1 
    
  */
{
  ddFlag(1,D,outFlag);
  return;
}

void
pointTally::setSecondDist(const double D) 
  /*!
    Sets the weighting system:
    \param D :: Second back track distance
  */
{
  ELog::EM<<"SECOND Distance:: "<<D<<ELog::endDiag;
  secondDist=D;
  secondDFlag=1;
  return;
}

void
pointTally::setCentre(const Geometry::Vec3D& PC)
  /*!
    Set the centre point
    \param PC :: Centre Point
  */
{
  Centre=PC;
  return;
}

void
pointTally::setCentre(const double D)
  /*!
    Given distance D set the centre based on 
    the window.
    \param D :: distance from centre 
  */
{
  ELog::RegMethod RegA("pointTally","setCentre(double)");

  if (Window.size()!=4)
   {
     ELog::EM<<"Tally ["<<this->IDnum
	      <<"] Window:Size [4] =="<<Window.size()<<ELog::endErr;
     return;
   }

  Geometry::Vec3D Mid;
  std::vector<Geometry::Vec3D>::const_iterator vc;
  for(vc=Window.begin();vc!=Window.end();vc++)
    Mid += *vc;
  Mid/=4.0;
  Geometry::Vec3D Norm=(Window[1]-Window[0])*(Window[3]-Window[0]);
  Norm.makeUnit();
  Centre=Norm*D+Mid;
  std::pair<Geometry::Vec3D,Geometry::Vec3D> Item=calcEdge();

  return;
}

Geometry::Vec3D
pointTally::calcMid() const 
  /*!
    Calculate the mid point
    \return Mid Point of Window
  */
{  
  ELog::RegMethod RegA("pointTally","calcMid");
  if (Window.empty())
    {
     ELog::EM<<"Tally ["<<this->IDnum
	     <<"] Window:Size [4] =="<<Window.size()
	     <<ELog::endErr;;
     throw ColErr::ExitAbort("Window empty");
    }

  Geometry::Vec3D Mid=
    std::accumulate(Window.begin(),Window.end(),
		    Geometry::Vec3D(0,0,0),
		    std::plus<Geometry::Vec3D>());
  Mid/=static_cast<double>(Window.size());
  return Mid;
}


double
pointTally::calcArea() const
  /*!
    Calculate the area of the window
    \return area
  */
{
  ELog::RegMethod RegA("pointTally","calcArea");
  if (Window.size()!=4)
   {
     ELog::EM.Estream()<<"Tally ["<<this->IDnum
		       <<"] Window:Size [4] =="<<Window.size();
     ELog::EM.error();
     return 1.0;
   }
  // Calculate the area from two cross products.
  const Geometry::Vec3D AO=Window[1]-Window[0];
  const Geometry::Vec3D BO=Window[2]-Window[0];
  double area=(AO*BO).abs();
  const Geometry::Vec3D CO=Window[0]-Window[3];
  const Geometry::Vec3D DO=Window[2]-Window[3];
  area+=(CO*DO).abs();
  return area/2.0;
}

Geometry::Vec3D
pointTally::calcNormal() const
  /*!
    Calculate the normal from the point tally window
    \return normal
  */
{
  ELog::RegMethod RegA("pointTally","calcNormal");
  if (Window.size()!=4)
   {
     ELog::EM<<"Tally ["<<this->IDnum
	     <<"] Window:Size [4] =="<<Window.size()<<ELog::endErr;
     throw ColErr::ExitAbort("Window size <4");
   }

  Geometry::Vec3D Norm=(Window[1]-Window[0])*(Window[3]-Window[0]);
  Norm.makeUnit();
  return Norm;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
pointTally::calcDirection() const
  /*!
    Calculate the direction vectors in the plane of the 
    view box.
    \return pair of vectors (0->1, 1->2)
  */
{
  std::pair<Geometry::Vec3D,Geometry::Vec3D> 
    Out=calcEdge();

  Out.first.makeUnit();
  Out.second.makeUnit();
  return Out;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
pointTally::calcEdge() const
  /*!
    Calculate the direction vectors in the plane of the 
    view box.
    \return pair of vectors (0->1, 1->2)
  */
{
  ELog::RegMethod RegA("pointTally","calcEdge");
  if (Window.size()!=4)
   {
     ELog::EM<<"Tally ["<<this->IDnum
	     <<"] Window:Size [4] =="<<Window.size()<<ELog::endErr;
     throw ColErr::ExitAbort(RegA.getFull());
   }
  std::pair<Geometry::Vec3D,Geometry::Vec3D> Out;
  Out.first=Window[1]-Window[0];
  Out.second=Window[2]-Window[1];
  return Out;
}

void
pointTally::setCentreAngle(const Geometry::Vec3D& PN,
			   const double D,const double A)
  /*!
    Given an angle A calculate and set the
    centre point D cm from the mid point of the
    window.
    \param PN :: Plane normal vector
    \param D :: distance from centre 
    \param A :: Angle from the normal
   */
{
  ELog::RegMethod RegA("pointTally","setCentreAngle");
  Geometry::Vec3D Mid=calcMid();
  Geometry::Vec3D Norm=calcNormal();
  Geometry::Vec3D AngleVec=PN*Norm;
  AngleVec.makeUnit();
  Centre=Norm*(D*cos(A))+AngleVec*(D*sin(A))+Mid;
  return;
}

void
pointTally::setWindow(const std::vector<Geometry::Vec3D>& Wvec)
  /*!
    Adds a point tally to the system
    \param Wvec :: List of points for the window
  */
{
  ELog::RegMethod RegA("pointTally","setWindow");

  Window=Wvec;
  orderWindow();
  return;
}

void
pointTally::orderWindow()
  /*!
    Sorts out a set of windows to have an rotation order 
  */
{
  ELog::RegMethod RegA("pointTally","orderWindow");

  if (Window.size()==4)
    {
      double D(0);
      size_t index(0);
      for(size_t i=1;i<4;i++)
        {
	  const double dval=Window[i].Distance(Window[0]);
	  if (dval>D)
	    {
	      index=i;
	      D=dval;
	    }
	}
      if (index!=2)
	std::swap(Window[2],Window[index]);
    }
  return;
}

void
pointTally::setCells(const std::vector<int>& cells)
  /*!
    Adds a set of cells ids to the point tally
    \param cells :: List of cell number [not currently checked ?]
  */
{
  fuCard.clear();
  fuCard.setVector(cells);
  return;
}

void
pointTally::printWindow() const 
  /*!
    Priont out the window [debug] function
  */
{
  const masterRotate& MR=masterRotate::Instance();

  for(size_t i=0;i<Window.size();i++)
    ELog::EM<<"W["<<i+1<<"] = "<<MR.calcRotate(Window[i])<<ELog::endTrace;
  return;
}

const Geometry::Vec3D&
pointTally::getWindowPt(const size_t Index) const
  /*!
    Get a window point from the rectangle
    \param Index :: Index point to obtain
    \return Window[Index]
  */
{
  ELog::RegMethod RegA("pointTally","getWindowPt");

  if (Index>=Window.size())
    throw ColErr::IndexError<size_t>
      (Index,Window.size(),RegA.getFull());

  return Window[Index];
}

Geometry::Plane
pointTally::calcPlane() const
  /*!
    Calculate the plane that the windows full into
    \return Plane
   */
{
  ELog::RegMethod RegA("pointTally","calcPlane");
  Geometry::Plane PA;
  PA.setPlane(Window[0],(Window[1]-Window[0])*(Window[2]-Window[1]));
  return PA;
}

void
pointTally::setProjectionCentre(const Geometry::Vec3D& Cp,
				const Geometry::Vec3D& Axis,
				const double Extra)
  /*!
    Calculate the intercept point of a line
    \param Cp :: Start point
    \param Axis :: Axis line
    \param Extra :: extra distance to project
   */
{
  ELog::RegMethod RegA("pointTally","setProjectionCentre");

  Geometry::Plane PA=calcPlane();
  MonteCarlo::LineIntersectVisit BeamLine(Cp,Axis);
  double D=BeamLine.getDist(&PA);
  D+=Extra;
  Centre=BeamLine.getTrack().getPoint(D);
  return;
}


void
pointTally::widenWindow(const int index,const double scale)
  /*!
    Widen window along a master direction (0/1)
    \param index :: direction (0/1)
    \param scale :: distance to move
  */
{
  ELog::RegMethod RegA("pointTally","widenWindow");

  if (index!=0 && index!=1)
    throw ColErr::IndexError<int>(index,2,"pointTally::windenWindow");      

  const std::pair<Geometry::Vec3D,Geometry::Vec3D> D=calcDirection();
  Geometry::Vec3D N=(!index) ? D.first : D.second;

  if (!index)
    {
      Window[0]-=N*scale;
      Window[1]+=N*scale;
      Window[2]+=N*scale;
      Window[3]-=N*scale;
    }
  else
    {
      Window[0]-=N*scale;
      Window[1]-=N*scale;
      Window[2]+=N*scale;
      Window[3]+=N*scale;
    }
  
  return;
}

void
pointTally::divideWindow(const int xPts,const int yPts)
  /*!
    Adjusts the tally to add a modification to the fu
    card which has xpts / ypts : NBins [numbered 1 to N ]
    \param xPts :: Xpoints in grid
    \param yPts :: Ypoints in grid
  */
{
  ELog::RegMethod RegA("pointTally","divideWindow");
  fuCard.clear();
  fuCard.addComp(xPts);
  fuCard.addComp(yPts);
  for(int i=1;i<=xPts*yPts;i++)
    fuCard.addComp(i);
  fuCard.condense();
  return;
}

void
pointTally::shiftWindow(const double distance)
  /*!
    Move the tally +/- along the line of the normal of the view
    \param distance :: distance to move
  */
{
  ELog::RegMethod RegA("pointTally","shiftWindow");

  for(size_t i=0;i<Window.size();i++)
    {
      const Geometry::Vec3D PUnit=(Centre-Window[i]).unit();
      Window[i]+=PUnit*distance;
    }
  return;
}

void
pointTally::slideWindow(const int index,const double scale)
  /*!
    Slide window along a master direction (0/1)
    \param index :: direction (0/1)
    \param scale :: distance to move
  */
{
  ELog::RegMethod RegA("pointTally","slideWindow");

  if (index!=0 && index!=1)
    throw ColErr::IndexError<int>(index,2,"pointTally::slideWindow");      

  if (Window.size()>=4)
    {
      const std::pair<Geometry::Vec3D,Geometry::Vec3D> D=calcDirection();
      Geometry::Vec3D N=(!index) ? D.first : D.second;
      Window[0]+=N*scale;
      Window[1]+=N*scale;
      Window[2]+=N*scale;
      Window[3]+=N*scale;
      
      ELog::EM<<"Window == "<<Window[0]<<" : "<<Window[1]<<" : "<<
	Window[2]<<" : "<<Window[3]<<ELog::endDiag;
    }
  return;
}

void
pointTally::removeWindow()
  /*!
    Removes the window
   */
{
  Window.clear();
  return;
}

int
pointTally::addLine(const std::string& LX)
  /*!
    Adds a string, if this fails
    it return Tally::addLine.
    Currently it only searches for:
    - f  : Initial line (then Centre + radius)
    - fu : Active line
    \param LX :: Line to search
    \retval -1 :: ID not correct
    \retval -2 :: Type object not correct
    \retval -3 :: Line not correct
    \retval 0 on success
    \retval Tally::addline(Line) if nothing found
  */
{
  ELog::RegMethod RegA("pointTally","addLine");

  std::string Line(LX);
  std::string Tag;
  if (!StrFunc::section(Line,Tag))
    return -1;

  // get tag+ number
  std::pair<std::string,int> FUnit=Tally::getElm(Tag);
  if (FUnit.second<0)
    return -2;

  if (FUnit.first=="f")
    {
      std::string Lpart=LX;
      Tally::processParticles(Lpart);        // pick up particles
      Geometry::Vec3D Pt;
      int i;
      for(i=0;i<3 && StrFunc::section(Lpart,Pt[i]);i++) ;
      if (i==3)
        {
	  Centre=Pt;
	  if (!StrFunc::section(Lpart,Radius) || Radius<0)
	    Radius=0.0;
	}
      return 0;
    }
  if (FUnit.first=="fu") 
    {
      
      fuCard.processString(Line);
      fuFlag=(fuCard.empty()) ? 1 : 0;
      return 0;
    }
  
  return Tally::addLine(LX);
}

void
pointTally::rotateMaster()
  /*!
    Rotate the points
  */
{
  ELog::RegMethod RegA("pointTally","rotateMaster");

  const masterRotate& MR=masterRotate::Instance();
  MR.applyFull(Centre);
  for_each(Window.begin(),Window.end(),
   	   [&MR](Geometry::Vec3D& v){MR.applyFull(v);});
  return;
}

void
pointTally::writeMCNP6(std::ostream& OX) const
  /*!
    Write out the window system in MCNP6 format
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("pointTally","writeMCNP6");
  
  if (Window.size()==4)   // anything else is not supported yet
    {
      std::ostringstream cx;
      cx.str("");
      cx<<"ft"<<IDnum;
      cx<< ((secondDFlag) ? " wdo " : " win ");
      
      cx<<Window[0];
      StrFunc::writeMCNPX(cx.str(),OX);
      for(size_t i=1;i<4;i++)
	{
	  cx.str("");
	  cx<<Window[i];
	  StrFunc::writeMCNPXcont(cx.str(),OX);
	}
      if (secondDFlag)
	{
	  cx.str("");
	  cx<<secondDist;
	  StrFunc::writeMCNPXcont(cx.str(),OX);
	} 
    }
  return;
}

void
pointTally::write(std::ostream& OX) const
  /*!
    Writes out the point tally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  ELog::RegMethod RegA("pointTally","write");

  if (!isActive())
    return;
  
  std::stringstream cx;
  if (IDnum)  // maybe default 
    {
      cx<<"f"<<IDnum;
      writeParticles(cx);
      cx<<Centre<<" "<<Radius;
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  if (!fuCard.empty() || fuFlag)
    {
      cx.str("");
      cx<<"fu"<<IDnum<<" "<<fuCard;
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  
  if ( !Window.empty() )
    {
      if ( !mcnp6Out )
        StrFunc::writeMCNPX(StrFunc::makeString("fu",IDnum),OX);
      else
        writeMCNP6(OX);
    }
            
  writeFields(OX);
  return;
}


}   // NAMESPACE tallySystem
