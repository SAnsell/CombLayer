/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/NodeList.cxx
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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "NodePoint.h"
#include "NodeList.h"


NodeList::NodeList()
  /*!
    Constructor
  */
{}

NodeList::NodeList(const NodeList& A) :
  Nodes(A.Nodes)
  /*!
    Standard Copy Constructor
    \param A :: NodeList to copy
  */
{}

NodeList&
NodeList::operator=(const NodeList& A) 
  /*!
    Standard Assigment operator
    \param A :: NodeList to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Nodes=A.Nodes;
    }  return *this;
}

NodeList::~NodeList()
  /*!
    Destructor
  */
{}

int
NodeList::readFile(const std::string& fname)
  /*!
    Reads the Node List file.
    Converts a [mm] file of X:Y:Z to cm 
    (by a mult 10)
    \param fname :: string used to opens file.
    \returns -ve on failure
    \retval 0 :: on success
  */
{
  ELog::RegMethod RegA("NodeList","readFile");

  std::ifstream IX;
  IX.open(fname.c_str(),std::ios::in);
  if (!IX.good())
    {
      ELog::EM<<"Unable to open file "<<fname<<ELog::endErr;
      return -1;
    }
  Nodes.clear();
  int idnum(0);
  double xd,yd,zd;
  ELog::EM<<"NodeList::read Expecting file with pts :: X Y Z [mm]"
	  <<ELog::endTrace;
  while(IX.good())
    {
      std::string Line=StrFunc::getLine(IX,255);
      std::string heldLine(Line);
      if (StrFunc::section(Line,xd) &&
	  StrFunc::section(Line,yd) && StrFunc::section(Line,zd))
        {
	  // Convert from mm to cm
	  Nodes.push_back(NodePoint(Geometry::Vec3D(xd/10.0,yd/10.0,zd/10.0),idnum));
	  idnum++;
	}
      else
	{
	  ELog::EM<<"NodeList::read Error reading Line :"
		  <<heldLine<<ELog::endErr;
	}
    }
  ELog::EM<<"NodeList::read Read to node ::"<<idnum<<ELog::endBasic;
  IX.close();
  return 0;
}

int
NodeList::vecNodes(const Geometry::Vec3D& Start,const Geometry::Vec3D& End,
		   const Geometry::Vec3D& Xn,const Geometry::Vec3D& Yn,
		   const Geometry::Vec3D& Zn)
  /*! 
    Sets a node boundary between Start and End
    using vector additions of Xn , Yn and Zn
    \param Start :: Lower corner of cube
    \param End :: Uppder corner of cube
    \param Xn :: X step vector 
    \param Yn :: Y step vector 
    \param Zn :: Z step vector 
    \returns number of nodes
  */
{
  int idum(0);
  Geometry::Vec3D S=Start;
  Geometry::Vec3D Direct=End-Start;
  Direct.reBase(Xn,Yn,Zn);
  const int Xpts=static_cast<int>(floor(fabs(Xn[0])));
  const int Ypts=static_cast<int>(floor(fabs(Xn[1]))); 
  const int Zpts=static_cast<int>(floor(fabs(Xn[2])));

  double scale[3];
  for(int i=0;i<3;i++)
    {
      scale[i]=(Xn[i]<0) ? -1.0 : 1.0;
      scale[i]*=fabs(Xn[i])/Xpts;
    }

  Geometry::Vec3D Tp;      //Test Point
  for(int i=0;i<Xpts;i++)
    for(int j=0;j<Ypts;j++)
      {
	Tp= Start+Xn*(i*scale[0])+Yn*(j*scale[1]);
	  for(int k=0;k<Zpts;k++)
	    {
	      Nodes.push_back(NodePoint(Tp,idum));
	      Tp+=Zn*scale[2];
	      idum++;
	    } 
      }
  return idum;
}

int
NodeList::cubeNodes(const Geometry::Vec3D& Start,const Geometry::Vec3D& End,const double XYZ)
  /*! 
     Sets up a cube of nodes between Start and End with spacing XYZ
     \param Start :: point on cube 
     \param End :: Point diagonally opposite on cube
     \param XYZ :: Step distance
     \return Number of nodes
  */
{
  int idum(0);
  double startPts[3];
  double endPts[3];
  for(int i=0;i<3;i++)
    {
      if (Start[i]>End[i])
        {
	  startPts[i]=End[i];
	  endPts[i]=Start[i];
	}
      else
        {
	  startPts[i]=Start[i];
	  endPts[i]=End[i];
	}
    }
  for(double Xpts=startPts[0];Xpts<endPts[0];Xpts+=XYZ)
    for(double Ypts=startPts[1];Ypts<endPts[1];Ypts+=XYZ)
      for(double Zpts=startPts[2];Zpts<endPts[2];Zpts+=XYZ)
        {
	  Nodes.push_back(NodePoint(Geometry::Vec3D(Xpts,Ypts,Zpts),idum));
	  idum++;
	}
  return idum;
}


int 
NodeList::eraseCellID(const std::vector<int>& CellID)
  /*! 
     Eliminates nodes in which the cellID is not 
     the current cell ID
     \param CellID :: Cell Ids of acceptable cells
     \return Number of cells eliminated
  */
{
  if (Nodes.empty())
    return 0;
  std::set<int> Active;
  std::insert_iterator<std::set<int> > ix(Active,Active.begin());
  copy(CellID.begin(),CellID.end(),ix);
  
  std::vector<NodePoint>::iterator insert_it=Nodes.begin();
  std::vector<NodePoint>::iterator loop_it=Nodes.begin();
  for(loop_it=Nodes.begin();loop_it!=Nodes.end();loop_it++)
    {
      if (Active.find(loop_it->getCellName())!=Active.end())
        {
	  if (insert_it!=loop_it)
	    (*insert_it)=(*loop_it);
	  insert_it++;
	}
    }

  Nodes.erase(insert_it,loop_it);
  return static_cast<int>(loop_it-insert_it);
}

void
NodeList::setTransform(const Geometry::Transform& Tl)
  /*!
    Sets the general transform to be applied to all points
    (but does not apply it)
    \param Tl :: Transform to apply
  */
{
  TC=Tl;
  return;
}

void
NodeList::processTrans()
  /*!
    Applies the transform to all the nodes points 
  */ 
{
  std::vector<NodePoint>::iterator nc;
  for(nc=Nodes.begin();nc!=Nodes.end();nc++)
    {
      nc->applyTransform(TC);
    }
  return;

}
void
NodeList::swapCoords(const int A,const int B)
  /*!
    Swaps the coordinates in the morph components
    \param A :: coordinate 1  [1-3]
    \param B :: coordinate 2  [1-3]
  */ 
{
  ELog::RegMethod RegA("NodeList","swapCoords");
  if (A==B || A>3 || B>3 
      || A<-3 || B<-3 || 
      !A || !B)
    {
      ELog::EM<<"Error in swapCoords"<<ELog::endErr;
      return;
    }

  const int signA=(A>0) ? 1 : -1;
  const int signB=(B>0) ? 1 : -1;
  if (signA*A==signB*B)
    {
      const int pt((signA*A)-1);
      std::vector<NodePoint>::iterator nc;
      for(nc=Nodes.begin();nc!=Nodes.end();nc++)
	nc->getMorph()[pt]*=-1;
      return;
    }
  // Swap (and maybe change)
  std::vector<NodePoint>::iterator nc;
  for(nc=Nodes.begin();nc!=Nodes.end();nc++)
    {
      const int ptA((signA*A)-1);
      const int ptB((signB*B)-1);
      Geometry::Vec3D& X=nc->getMorph();
      double tmp=X[ptA];
      X[ptA]=signB*X[ptB];
      X[ptB]=signA*tmp;
    }
  return;
}

void
NodeList::write(const std::string& Fname) const
  /*!
    Writes out the data to a file 
    \param Fname :: Data file name
  */
{
  ELog::RegMethod RegA("NodeList","write");

  std::ofstream OX;
  OX.open(Fname.c_str(),std::ios::out);
  std::vector<NodePoint>::const_iterator nc;
  for(nc=Nodes.begin();nc!=Nodes.end();nc++)
    {
      nc->write(OX);
      // Error trap
      if (!OX.good())
        {
	  ELog::EM<<"Unable to access file ::"<<Fname
		   <<" at "<<static_cast<int>(nc-Nodes.begin())+1
		  <<ELog::endErr;
	  OX.close();
	  return;
	}
    }
  // General catch all
  OX.close();
  return;
}

void
NodeList::applyPressure(const Geometry::Vec3D& Apt,const double Force)
  /*!
    Applies pressure to all NodePoints
    \param Apt :: Point that pressure is considered to come from
    \param Force :: scalar value for the pressure
  */
{
  std::vector<NodePoint>::iterator vp;
  for(vp=Nodes.begin();vp!=Nodes.end();vp++)
    {
      vp->applyPressure(Apt,Force);
    }
  return;
}

double
NodeList::nodeScore(const int AimMat) const
  /*!
    Get a simple score based on number of point
    which are in the correct material type
    \param AimMat :: Materail determine if in correct place
    \returns Sum of correct nodes
  */
{
  double score(0.0);
  std::vector<NodePoint>::const_iterator vp;
  for(vp=Nodes.begin();vp!=Nodes.end();vp++)
    {
      score+= (vp->getMatType()!=AimMat) ? 1.0 : 0.0;
    }
  return score;
}

void
NodeList::scale(const double S)
  /*!
    Scale each point by S
    \param S :: Value to scale the nodes by
  */
{
  for_each(Nodes.begin(),Nodes.end(),
	   std::bind2nd(std::mem_fun_ref(&NodePoint::scale),S));
  return;
}
