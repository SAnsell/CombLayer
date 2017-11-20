/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/Source.cxx 
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
#include <sstream>
#include <complex>
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MapSupport.h"
#include "mathSupport.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "Source.h"

namespace SDef
{

void
Source::populate()
  /*!
    Populate of possibl items in a source card and appropiate value
  */
{
  ELog::RegMethod RegA("Source","populate");

  sdMap.insert(sdMapTYPE::value_type("cel",SBasePtr(new SrcItem<int>("cel"))));
  sdMap.insert(sdMapTYPE::value_type("sur",SBasePtr(new SrcItem<int>("sur"))));
  sdMap.insert(sdMapTYPE::value_type("erg",SBasePtr(new SrcItem<double>("erg"))));
  sdMap.insert(sdMapTYPE::value_type("tme",SBasePtr(new SrcItem<double>("tme"))));
  sdMap.insert(sdMapTYPE::value_type("dir",SBasePtr(new SrcItem<double>("dir"))));
  sdMap.insert(sdMapTYPE::value_type("vec",SBasePtr(new SrcItem<Geometry::Vec3D>("vec"))));
  sdMap.insert(sdMapTYPE::value_type("nrm",SBasePtr(new SrcItem<double>("nrm"))));
  sdMap.insert(sdMapTYPE::value_type("pos",SBasePtr(new SrcItem<Geometry::Vec3D>("pos"))));
  sdMap.insert(sdMapTYPE::value_type("rad",SBasePtr(new SrcItem<double>("rad"))));
  sdMap.insert(sdMapTYPE::value_type("ext",SBasePtr(new SrcItem<double>("ext"))));  
  sdMap.insert(sdMapTYPE::value_type("axs",SBasePtr(new SrcItem<Geometry::Vec3D>("axs"))));
  sdMap.insert(sdMapTYPE::value_type("x",SBasePtr(new SrcItem<double>("x"))));
  sdMap.insert(sdMapTYPE::value_type("y",SBasePtr(new SrcItem<double>("y"))));
  sdMap.insert(sdMapTYPE::value_type("z",SBasePtr(new SrcItem<double>("z"))));
  sdMap.insert(sdMapTYPE::value_type("ccc",SBasePtr(new SrcItem<int>("ccc"))));
  sdMap.insert(sdMapTYPE::value_type("ara",SBasePtr(new SrcItem<double>("ara"))));
  sdMap.insert(sdMapTYPE::value_type("wgt",SBasePtr(new SrcItem<double>("wgt"))));
  sdMap.insert(sdMapTYPE::value_type("eff",SBasePtr(new SrcItem<double>("eff"))));
  sdMap.insert(sdMapTYPE::value_type("par",SBasePtr(new SrcItem<int>("par"))));
  sdMap.insert(sdMapTYPE::value_type("tr",SBasePtr(new SrcItem<int>("tr"))));
  return;
}

Source::Source() : 
  transPTR(0)
  /*!
    Constructor
  */
{
  populate();
}

Source::Source(const Source& A) :
  transPTR(A.transPTR),
  sdMap(A.sdMap),DVec(A.DVec)
  /*!
    Copy Constructor
    \param A :: Source to copy
  */
{ }

Source&
Source::operator=(const Source& A)
  /*!
    Assignment operator
    \param A :: Source to copy
    \return *this
  */
{
  if (this!=&A)
    {
      transPTR=A.transPTR;
      sdMap=A.sdMap;
      DVec=A.DVec;
    }
  return *this;
}

Source::~Source()
  /*!
    Destructor
  */
{}


SrcBase*
Source::getBase(const std::string& Key)
  /*!
    Get a specific SrcItem based on the key
    \param Key :: Key to find
    \return SrcItem
  */
{
  sdMapTYPE::iterator mc=sdMap.find(Key);
  return (mc==sdMap.end()) ? 0 : mc->second.get();
}

template<typename T>
SrcItem<T>*
Source::getItem(const std::string& Key)
  /*!
    Get a specific SrcItem based on the key
    \param Key :: Key to find
    \return SrcItem
  */
{
  sdMapTYPE::iterator mc=sdMap.find(Key);
  return (mc==sdMap.end()) ? 0 :
    dynamic_cast<SrcItem<T>* > (mc->second.get());  
}

template<typename T>
const SrcItem<T>*
Source::getItem(const std::string& Key) const
  /*!
    Get a specific SrcItem based on the key
    \param Key :: Key to find
    \return SrcItem
  */
{
  sdMapTYPE::const_iterator mc=sdMap.find(Key);
  return (mc==sdMap.end()) ? 0 :
    dynamic_cast<const SrcItem<T>* > (mc->second.get());  
}


template<typename T>
void
Source::setComp(const std::string& Key,const T& Item)
  /*!
    Set a particular value. Note the care in moving up the ladder
    \param Key :: Key to find
    \param Item :: Value to set
  */
{
  ELog::RegMethod RegA("Source","setComp");
  SrcItem<T>* SPtr=getItem<T>(Key);
  if (!SPtr)
    throw ColErr::InContainerError<std::string>(Key,RegA.getFull());
  SPtr->setValue(Item);
  return;
}

void
Source::setData(const std::string& Key,const SrcData& DObj)
  /*!
    Set a particular value. Note the care in moving up the ladder
    \param Key :: Key to find
    \param DObj :: SrcData object
  */ 
{
  ELog::RegMethod RegA("Source","setData");
  SrcBase* SPtr=getBase(Key);
  if (!SPtr)
    throw ColErr::InContainerError<std::string>(Key,"Source::setData");
  if (DObj.isDependent())
    {
      ELog::EM<<"Setting dependent type "<<DObj.getDep()<<ELog::endWarn;
      SPtr->setDepType(DObj.getDep(),static_cast<int>(DObj.getIndex()));
    }
  else
    SPtr->setDataType(static_cast<int>(DObj.getIndex()));

  DVec.push_back(SDataPtr(new SrcData(DObj)));
  // Sort ??
  return;
}

void
Source::addComp(const std::string& Key,const SrcBase* BPtr)
  /*!
    Add a string
    \param Key :: Key name to add
    \param BPtr :: Pointer to SrcBase
  */
{
  sdMapTYPE::iterator mc=sdMap.find(Key);
  if (mc!=sdMap.end())
    mc->second=SBasePtr(BPtr->clone());
  else
    sdMap.insert(sdMapTYPE::value_type(Key,SBasePtr(BPtr->clone())));
  return;
}


void
Source::cleanGroups()
  /*!
    Find pairs/triples and clean up the SP/SI/SB groups
    remove initial z    
    \todo TO BE DONE
  */
{
  return;
}

void
Source::cutEnergy(const double Ecut)
  /*!
    Cut the energy 
    \param Ecut :: Energy
   */
{
  ELog::RegMethod RegA("Source","cutEnergy");

  sdMapTYPE::iterator mc=sdMap.find("erg");
  if (mc!=sdMap.end())
    {
      const int dtype=mc->second->getDataType();
      // dvecTYPE ==> shared_ptr<SrcData>
      dvecTYPE::iterator vc=
	find_if(DVec.begin(),DVec.end(),
		[dtype](dvecTYPE::value_type& SD) -> bool 
		{ return (static_cast<int>(SD->getIndex())==dtype);  } );
      if (vc!=DVec.end())
	vc->get()->cutValue(Ecut);
    }
  return;
}

void
Source::substituteCell(const int originalCell,const int newCell)
  /*!
    Substitute Cell
    \param originalCell :: original cell number
    \param newCell :: new cell number
  */
{
  ELog::RegMethod RegA("Source","substituteCell");

  const char* keyName[2]={"cel","ccc"};
  for(int i=0;i<2;i++)
    {
      sdMapTYPE::iterator mc=sdMap.find(keyName[i]);
      if (mc!=sdMap.end()) 
	{
	  SrcItem<int>* SI=dynamic_cast< SrcItem<int>* >(mc->second.get());
	  if (SI && SI->isData() && SI->getData()==originalCell)
	    SI->setValue(newCell);
	}
    }
  return;
}

void
Source::substituteSurface(const int originalSurface,const int newSurface)
  /*!
    Substitute Surface
    \param originalSurface :: original surface
    \param newSurface :: new surface
  */
{
  sdMapTYPE::iterator mc=sdMap.find("sur");
  if (mc!=sdMap.end()) 
    {
      SrcItem<int>* SI=dynamic_cast< SrcItem<int>* >(mc->second.get());
      if (SI && SI->isData() && SI->getData()==originalSurface)
	SI->setValue(newSurface);
    }
  return;
}

int
Source::rotateMaster()
  /*!
    Execute a rotation using the masterRotation 
    This is a mess: The code rotates the main axes x,y,z and then
    sees were they lie. If on another axis then that is ok and 
    it is accected. 
    \return Need to create Source Transform first [1]
  */
{
  ELog::RegMethod RegA("Source","rotateMaster");

  // Place for general output:
  typedef  std::vector<std::pair<std::string,SBasePtr> > OutTYPE;
  OutTYPE Out;

  masterRotate& MR=masterRotate::Instance();
  const char xyz[3][2]={"x","y","z"};
  Geometry::Vec3D XYZ[3];
  XYZ[0]=Geometry::Vec3D(1,0,0);
  XYZ[1]=Geometry::Vec3D(0,1,0);
  XYZ[2]=Geometry::Vec3D(0,0,1);

  // Do all rotations first since needed for transform matrix
  for(int i=0;i<3;i++)
    XYZ[i]=MR.calcAxisRotate(XYZ[i]);

  for(int i=0;i<3;i++)
    {
      sdMapTYPE::iterator mc=sdMap.find(std::string(xyz[i]));
      if (mc!=sdMap.end() && mc->second->isActive())
	{
	  const int aR=XYZ[i].masterDir();
	  if (aR != i)
	    {
	      if (!transPTR) return 1;
	      // Now calculate the rotation from the point : 
	      setTransform(XYZ);
	      // Note don't change x,y,z but rotate whole system:
	      return 0;
	    }
	}
    }
  for(int i=0;i<3;i++)
    {
      sdMapTYPE::iterator mc=sdMap.find(std::string(xyz[i]));
      if (mc!=sdMap.end() && mc->second->isActive())
	{
	  const int aR=XYZ[i].masterDir();
	  if (aR)
	    {
	      Out.push_back(OutTYPE::value_type(std::string(xyz[abs(aR)-1]),
						SBasePtr(mc->second->clone())));
	      SrcItem<double>* valPtr=
		dynamic_cast<SrcItem<double>*>(Out.back().second.get());
	      if (valPtr && aR<0 && valPtr->isData())
		valPtr->setValue(-1.0*valPtr->getData());
	      // Clear incase not set:
	      mc->second=SBasePtr(new SrcItem<double>(xyz[i]));
	    }
	}
    }

  // Place new objects into original Map
  OutTYPE::iterator ovc; 
  for(ovc=Out.begin();ovc!=Out.end();ovc++)
    {
      sdMapTYPE::iterator mc=sdMap.find(ovc->first);
      SrcItem<double>* Mptr=dynamic_cast<SrcItem<double>* >(mc->second.get());
      SrcItem<double>* Cptr=dynamic_cast<SrcItem<double>* >(ovc->second.get());
      if (!Mptr)
	throw ColErr::CastError<SrcBase>(mc->second.get(),"Mptr Source");
      if (!Cptr)
	throw ColErr::CastError<SrcBase>(ovc->second.get(),"Ctr Source");
      (*Mptr)=(*Cptr);
    }

  // Now rotate the vectors:
  const std::vector<std::string> Keys({"vec","axs","pos"});
  for(const std::string& KItem : Keys)
    {
      sdMapTYPE::iterator mc=sdMap.find(KItem);
      if (mc!=sdMap.end() && mc->second->isActive())
	{
	  SrcItem<Geometry::Vec3D>* Mptr=
	    dynamic_cast<SrcItem<Geometry::Vec3D>* >(mc->second.get());
	  if (Mptr && Mptr->isData())
	    {
	      Geometry::Vec3D Pt=Mptr->getData();
	      MR.applyFull(Pt);
	      Mptr->setValue(Pt);
	    }
	}
    }
  return 0;
  
}

Geometry::Vec3D
Source::getPosCentre() const
  /*!
    Get the centre position
    \return (000) if undefinded / position otherwize
   */
{
  const SrcItem<Geometry::Vec3D>* SPtr=
    getItem<Geometry::Vec3D>("pos");
  if (!SPtr || !SPtr->isActive() || !SPtr->isData())
    return Geometry::Vec3D(0,0,0);
  return SPtr->getData();
}

Geometry::Vec3D
Source::getAxisLine() const
  /*!
    Get the central direction
    \return (010) if undefinded / position otherwize
   */
{
  const SrcItem<Geometry::Vec3D>* SPtr=
    getItem<Geometry::Vec3D>("vec");
  if (!SPtr || !SPtr->isActive() || !SPtr->isData())
    return Geometry::Vec3D(0,1,0);
  return SPtr->getData();
}

void
Source::setTransform(const Geometry::Vec3D XYZ[3])
  /*!
    Construct the transform from the rotation
    \param XYZ :: XYZ rotations
  */
{
  ELog::RegMethod RegA("Source","setTransform");

  masterRotate& MR=masterRotate::Instance();
  Geometry::Matrix<double> A(3,3);
  for(size_t i=0;i<3;i++)
    for(size_t j=0;j<3;j++)
      A[i][j]=XYZ[i][j]; 
  

  Geometry::Vec3D Origin=getPosCentre();
  Geometry::Vec3D Axis=getAxisLine();
  MR.calcAxisRotate(Axis);
  MR.calcRotate(Origin);
  
  transPTR->setTransform(Origin,A);  
  return;
}

void
Source::clear()
  /*!
    The big clearance call.
    Slightly excessive
  */
{
  DVec.clear();
  sdMap.erase(sdMap.begin(),sdMap.end());
  populate();
}

void
Source::write(std::ostream& OX) const
  /*!
    Write out the sdef card
    \param OX :: Output Stream
  */
{
  std::string out("sdef ");
  sdMapTYPE::const_iterator mc;
  for(mc=sdMap.begin();mc!=sdMap.end();mc++)
    out+=mc->second->getString();
  
  if (transPTR)
    out+=StrFunc::makeString(std::string(" tr="),
			     transPTR->getName());
  
  StrFunc::writeMCNPX(out,OX);
  
  for_each(DVec.begin(),DVec.end(),
	   std::bind(&SrcData::write,std::placeholders::_1,std::ref(OX)));
  return;
}


///\cond TEMPLATE

template SrcItem<double>* Source::getItem(const std::string&);
template SrcItem<int>* Source::getItem(const std::string&);
template SrcItem<Geometry::Vec3D>* Source::getItem(const std::string&);

template const SrcItem<double>* Source::getItem(const std::string&) const;
template const SrcItem<int>* Source::getItem(const std::string&) const;
template const SrcItem<Geometry::Vec3D>* Source::getItem(const std::string&) const;


template void Source::setComp(const std::string&,const double&);
template void Source::setComp(const std::string&,const int&);
template void Source::setComp(const std::string&,const Geometry::Vec3D&);

///\endcond TEMPLATE

}  // NAMESPACE SDef


