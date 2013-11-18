/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/Radiation.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Tensor.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Radiation.h"


Radiation::Radiation() : coordinate(0),current(1.0)
 ///< Constructor
{ }
 
Radiation::Radiation(const Radiation& A) :
  coordinate(A.coordinate),current(A.current),
  Xpts(A.Xpts),Ypts(A.Ypts),Zpts(A.Zpts),
  Upts(A.Upts)
/*!
  Copy constructor
  \param A :: Radiation object to copy
*/
{ }

Radiation&
Radiation::operator=(const Radiation& A) 
  /*!
    Assignment operator, copies the whole class
    \param A :: Radiatoin component to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      coordinate=A.coordinate;
      current=A.current;
      Xpts=A.Xpts;
      Ypts=A.Ypts;
      Zpts=A.Zpts;
      Upts=A.Upts;
    }
  return *this;
}

Radiation::~Radiation()
///< Destructor
{}

int
Radiation::openMcnpx(const std::string& fname,
		     std::ifstream& IFS,unsigned int* Pts) 
  /*! 
     Ppens the Mcnpx file and gets the first sections 
     Pts contains the number of X,Y,Z pts to be read 
     \param fname :: File name 
     \param IFS :: Input stream
     \param Pts :: Output array
     \retval +ve :: on error
     \retval 0  :: on success
  */
{
  ELog::RegMethod RegA("Radiation","openMcnpx");

  char ss[256];
  IFS.open(fname.c_str(),std::ios::in);
  if (!IFS.good())
    {
      ELog::EM<<"Unable to open file "<<fname<<ELog::endErr;
      return 2;
    }

  IFS.ignore(255,'\n');         // Title line
  IFS.getline(ss,255,'\n');     // number line 
  IFS.ignore(255,'\n');         // 0 line
  int Nsect;
  if (!IFS.good() || !StrFunc::convert(ss,Nsect))
    { 
      ELog::EM<<"Error with sectors :: radiation file"<<ELog::endErr;
      return 3;
    }
  IFS.getline(ss,255,'\n');    
  int pt[5];
  int icnt;
  std::string Line(ss);
  for(icnt=0;icnt<5 && StrFunc::section(Line,pt[icnt]);icnt++) ;
  if (icnt!=5)
    {
      ELog::EM<<"Error with points :: Radiation file"
	       <<ELog::endErr;
      return 4;
    }  
  // Set points for output
  if (Pts)
    for(int i=0;i<3;i++)
      Pts[i]=(pt[i+2]<1) ? 0 : static_cast<unsigned int>(pt[i+2]-1);
      
  coordinate=0;

  if (pt[0]==22)
    coordinate=1;
  else if (pt[0]!=12)
    std::cerr<<"Unknown coordinate system using rectangular"<<std::endl;

  IFS.ignore(255,'\n');               //size limits 
  IFS.ignore(255,'\n');               // 0   
  IFS.ignore(255,'\n');               // 0

  return 0;
}


int 
Radiation::readFile(const std::string& Fname)
  /*!
    Reads a grid.total file 
    - currently only reads the first section 
    \param Fname :: File to read
    \retval 0 :: on successful 
    \retval -ve :: Error
  */

{
  ELog::RegMethod RegA("Radiation","readFile");
  std::ifstream IFS;
  unsigned int Pts[3];
  if (openMcnpx(Fname,IFS,Pts))
    return -1;

  // Resize points
  Xpts.resize(Pts[0]+1);
  Ypts.resize(Pts[1]+1);
  Zpts.resize(Pts[2]+1);
  
// This should be repeated for each sector.

  std::string pLine;            // actual splitable line
  std::string excLine;          // excess unit
  double td;                    // double for readin tin
  
// READ XPTS LINE


  for(int i=0;i<3;i++)
    {
      std::vector<double>::iterator vc;
      vc=(i==0) ? Xpts.begin() : 
	(i==1) ? Ypts.begin() : Zpts.begin();

      int plret=StrFunc::getPartLine(IFS,pLine,excLine);
      unsigned int cnt(0);
      while(cnt<=Pts[i])               // Points list has first + last bin 
        {
	  if (StrFunc::section(pLine,td))
	    {
	      (*vc)=td;
	      vc++;
	      cnt++;
	    }
	  else if (plret)
	    {
	      pLine=excLine;
	      plret=StrFunc::getPartLine(IFS,pLine,excLine);
	    }
	  else
	    {
	      ELog::EM<<"XYZ"[i]<<"Pts insufficient "<<ELog::endErr;
	      return -2;
	    }
	}
    }



//CREATE SPACE -- --
      
  Upts.setMem(Pts[0],Pts[1],Pts[2]);
      
  // Now read whole files 
// Points distributed as X on the horrizontal 
// Y increase then Z increase

  int plret;
  size_t ia(0),ib(0),ic(0);      // three x,y,z counters

  while(ic<Pts[2])
    {
      plret=StrFunc::getPartLine(IFS,pLine,excLine);
      while(plret>0)             // read cont line
	{
	  while (ia<Pts[0] && StrFunc::section(pLine,td))
	    {
	      Upts[ia][ib][ic]=td;
	      cycleUpdate(ia,ib,ic);
	    }
	  pLine=excLine;
	  plret=StrFunc::getPartLine(IFS,pLine,excLine);
	}
      if (!plret)
	while (ic<Pts[2] && StrFunc::section(pLine,td))
	  {
	    Upts[ia][ib][ic]=td;
	    cycleUpdate(ia,ib,ic);
	  }
      
      pLine="";
      if (plret<0)
	{
	  ELog::EM<<"Failed Read Point:: "<<ia<<":"<<ib<<":"<<ic<<
	    ELog::endErr;
	  return -3;
	}
    }
  IFS.close();
  ELog::EM<<"Read Pts "<<ia+Pts[0]*(ib+ic*Pts[1])<<" from "<<
    Pts[0]*Pts[1]*Pts[2]<<ELog::endDiag;
  return 0;
}
  
void
Radiation::cycleUpdate(size_t& IA,size_t& IB,size_t& IC) const
  /*!
    With three variables allows a simle cycle update 
    based on the size of the Xpts,Ypts and Zpts vectors
    \param IA :: Low bit count
    \param IB :: Mid bit count
    \param IC :: High bit count
  */
{
  IA++;
  if (IA!=Xpts.size()-1)
    return;
  IA=0;
  IB++;
  if (IB!=Ypts.size()-1)
    return;
  IB=0;
  IC++;
  return;
}

double
Radiation::heat(const Geometry::Vec3D& Ptx) const
  /*!
    Calculates the head at a given point 
    \param Ptx :: Point to get heat at (rectangular coordinates)
    \return Watt/cc^3
  */

{ 
  ELog::RegMethod RegA("Radiation","heat");

  Geometry::Vec3D Pt((coordinate) ? toCylinderCoord(Ptx) : Ptx);

  std::vector<double>::const_iterator mn;

  const std::vector<double>* XYZ[3]={&Xpts,&Ypts,&Zpts};
  size_t idXYZ[3];
  for(size_t i=0;i<3;i++)
    {
      mn=lower_bound(XYZ[i]->begin(),XYZ[i]->end(),Pt[i]);
      if (mn==XYZ[i]->end() || XYZ[i]->front()>Pt[i])
	return 0.0;
      idXYZ[i]=static_cast<size_t>(mn-XYZ[i]->begin());
      if (idXYZ[i]>0)
	idXYZ[i]--;
    }
  
  // Do we need to calculate volume ????  -- I think no
  //  const double Vlx((Xpts[idx+1]-Xpts[idx])*
  //		   (Ypts[idy+1]-Ypts[idy])*
  //		   (Zpts[idz+1]-Zpts[idz]));

  return Upts[idXYZ[0]][idXYZ[1]][idXYZ[2]]*current;
}

void
Radiation::setRange(double* Low,double* High) const
  /*!
    Outputs the range over which the radiation data exist
    \param Low :: Pointer of type double[3]
    \param High :: Pointer of type double[3]
   */
 {
  Low[0]=Xpts.front();
  Low[1]=Ypts.front();
  Low[2]=Zpts.front();
  High[0]=Xpts.back();
  High[1]=Ypts.back();
  High[2]=Zpts.back();
  return;
}

double
Radiation::Volume() const
  /*!
    \return the volume of the radiation box.
  */
{
  double V=Xpts.back()-Xpts.front();
  V*=Ypts.back()-Ypts.front();
  V*=Zpts.back()-Zpts.front();
  return V;
}

Geometry::Vec3D
Radiation::toCylinderCoord(const Geometry::Vec3D& Px) const
  /*!
    converts Px (x,y,z) to cylinder coordinates 
    cylinder aligned along z axis.
    the angles start from x-axis
    \param Px :: Point to calcuated in cylindrical coordinates
    \return Point 
  */
{
  Geometry::Vec3D Pout;                    //Radial points 
  Pout[1]=Px[2];                         // Zaxis
  Pout[0]=sqrt(Px[1]*Px[1]+Px[0]*Px[0]);  // Radius
  Pout[2]=180.0*atan2(Px[1],Px[0])/M_PI;              //angle
  if (Pout[2]<0.0)
    Pout[2]=180.0-Pout[2];
  return Pout;
}
