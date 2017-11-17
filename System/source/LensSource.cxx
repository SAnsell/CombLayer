/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/LensSource.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "doubleErr.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "DSTerm.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "ModelSupport.h"
#include "WorkData.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "SourceBase.h"
#include "LensSource.h"

namespace SDef
{

LensSource::LensSource(const std::string& Key) : 
  attachSystem::FixedOffset(Key,0),SourceBase(),
  radialArea(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

LensSource::LensSource(const LensSource& A) : 
  attachSystem::FixedOffset(A),SourceBase(A),
  radialArea(A.radialArea)
  /*!
    Copy constructor
    \param A :: LensSource to copy
  */
{}

LensSource&
LensSource::operator=(const LensSource& A)
  /*!
    Assignment operator
    \param A :: LensSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      SourceBase::operator=(A);
      radialArea=A.radialArea;
    }
  return *this;
}

LensSource::~LensSource() 
  /*!
    Destructor
  */
{}

LensSource*
LensSource::clone() const
  /*!
    Clone constructor
    \return copy of this
  */
{
  return new LensSource(*this);
}

void
LensSource::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variables to access
  */
{
  ELog::RegMethod RegA("LensSource","populate");

  FixedOffset::populate(Control);
  SourceBase::populate(keyName,Control);
  radialArea=Control.EvalDefVar<double>(keyName+"Radial",0.0);
  
  return;
}


void
LensSource::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linear Component to represent proton beam
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("LensSource","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);  
  applyOffset();
  
  return;
}


void
LensSource::createAll(const FuncDataBase& Control,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param Control :: DataBase resouces
    \param FC :: Proton beam line 
    \param sideIndex :: side index
  */
{
  ELog::RegMethod RegA("LensSource","createAll");

  populate(Control);
  createUnitVector(FC,sideIndex);
  
  return;
}

void
LensSource::rotate(const localRotate& LR)
  /*!
    Rotate the source
    \param LR :: Rotation to apply
  */
{
  ELog::RegMethod Rega("LensSource","rotate");
  FixedComp::applyRotation(LR);  
  return;
}
  
  
void
LensSource::createSource(SDef::Source& sourceCard) const
  /*!
    Creates a lens neutron source [7MeV]
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("LensSource","createSource");

  sourceCard.setComp("par",particleType);   // neutron (1)/photon(2)
  sourceCard.setComp("vec",Y);
  sourceCard.setComp("axs",Y);
  sourceCard.setComp("ara",M_PI*radialArea*radialArea);         
    
  sourceCard.setComp("x",Origin[0]);
  sourceCard.setComp("y",Origin[1]);
  sourceCard.setComp("z",Origin[2]);


  // Radial
  if (radialArea>Geometry::zeroTol)
    {
      SDef::SrcData R1(3);
      SDef::SrcInfo* SIR3=R1.getInfo();
      SDef::SrcProb* SPR3=R1.getProb();
      SIR3->addData(0.0);
      SIR3->addData(radialArea);
      SPR3->setFminus(-21,1.0);
      sourceCard.setData("rad",R1);
    }

  // Direction:  
  const double angles[]=
    {-1.000e+00,-9.848e-01,-8.660e-01,-6.428e-01,-3.420e-01,
     0.000e+00,3.420e-01,6.428e-01,8.660e-01,9.848e-01,
     1.000e+00};
  const double anglesP[]=
    { 0.000E+00,5.969E+01,4.895E+02,9.452E+02,1.322E+03,
      1.267E+03,1.379E+03,1.627E+03,1.647E+03,9.364E+02,
      1.244E+02 };
  
  SDef::SrcData D1(1);
  SDef::SrcInfo* SI1=D1.getInfo();
  SDef::SrcProb* SP1=D1.getProb();
  for(unsigned int i=0;i<sizeof(angles)/sizeof(double);i++)
    {
      SI1->addData(angles[i]);
      SP1->addData(anglesP[i]);
    }
  sourceCard.setData("dir",D1);  

  const size_t NEnergy(10);
  const double energy[NEnergy][56]={
    {
    1.0e-02,6.000E-02,1.200E-01,1.800E-01,2.400E-01,3.000E-01,
    3.600E-01,4.200E-01,4.800E-01,5.400E-01,6.000E-01,
    6.600E-01,7.200E-01,7.800E-01,8.400E-01,9.000E-01,
    9.600E-01,1.020E+00,1.080E+00,1.140E+00,1.200E+00,
    1.260E+00,1.320E+00,1.380E+00,1.440E+00,1.500E+00,
    1.560E+00,1.620E+00,1.680E+00,1.740E+00,1.800E+00,
    1.860E+00,1.920E+00,1.980E+00,2.040E+00,2.100E+00,
    2.160E+00,2.220E+00,2.280E+00,2.340E+00,2.400E+00,
    2.460E+00,2.520E+00,2.580E+00,2.640E+00,2.700E+00,
    2.760E+00,2.820E+00,2.880E+00,2.940E+00,3.000E+00,
    3.060E+00,3.120E+00,3.180E+00,3.240E+00,3.300E+00
    },
    {
    1.0e-02,6.000E-02,1.200E-01,1.800E-01,2.400E-01,3.000E-01,
    3.600E-01,4.200E-01,4.800E-01,5.400E-01,6.000E-01,
    6.600E-01,7.200E-01,7.800E-01,8.400E-01,9.000E-01,
    9.600E-01,1.020E+00,1.080E+00,1.140E+00,1.200E+00,
    1.260E+00,1.320E+00,1.380E+00,1.440E+00,1.500E+00,
    1.560E+00,1.620E+00,1.680E+00,1.740E+00,1.800E+00,
    1.860E+00,1.920E+00,1.980E+00,2.040E+00,2.100E+00,
    2.160E+00,2.220E+00,2.280E+00,2.340E+00,2.400E+00,
    2.460E+00,2.520E+00,2.580E+00,2.640E+00,2.700E+00,
    2.760E+00,2.820E+00,2.880E+00,2.940E+00,3.000E+00,
    3.060E+00,3.120E+00,3.180E+00,3.240E+00,3.300E+00
    },
    {
    1.0e-02,6.400E-02,1.270E-01,1.910E-01,2.550E-01,3.180E-01,
    3.820E-01,4.450E-01,5.090E-01,5.730E-01,6.360E-01,
    7.000E-01,7.640E-01,8.270E-01,8.910E-01,9.550E-01,
    1.018E+00,1.082E+00,1.145E+00,1.209E+00,1.273E+00,
    1.336E+00,1.400E+00,1.464E+00,1.527E+00,1.591E+00,
    1.655E+00,1.718E+00,1.782E+00,1.845E+00,1.909E+00,
    1.973E+00,2.036E+00,2.100E+00,2.164E+00,2.227E+00,
    2.291E+00,2.355E+00,2.418E+00,2.482E+00,2.545E+00,
    2.609E+00,2.673E+00,2.736E+00,2.800E+00,2.864E+00,
    2.927E+00,2.991E+00,3.055E+00,3.118E+00,3.182E+00,
    3.245E+00,3.309E+00,3.373E+00,3.436E+00,3.500E+00
    },
    {
    1.0e-02,6.700E-02,1.350E-01,2.020E-01,2.690E-01,3.360E-01,
    4.040E-01,4.710E-01,5.380E-01,6.050E-01,6.730E-01,
    7.400E-01,8.070E-01,8.750E-01,9.420E-01,1.009E+00,
    1.076E+00,1.144E+00,1.211E+00,1.278E+00,1.345E+00,
    1.413E+00,1.480E+00,1.547E+00,1.615E+00,1.682E+00,
    1.749E+00,1.816E+00,1.884E+00,1.951E+00,2.018E+00,
    2.085E+00,2.153E+00,2.220E+00,2.287E+00,2.355E+00,
    2.422E+00,2.489E+00,2.556E+00,2.624E+00,2.691E+00,
    2.758E+00,2.825E+00,2.893E+00,2.960E+00,3.027E+00,
    3.095E+00,3.162E+00,3.229E+00,3.296E+00,3.364E+00,
    3.431E+00,3.498E+00,3.565E+00,3.633E+00,3.700E+00
    },
    {
    1.0e-02,7.300E-02,1.450E-01,2.180E-01,2.910E-01,3.640E-01,
    4.360E-01,5.090E-01,5.820E-01,6.550E-01,7.270E-01,
    8.000E-01,8.730E-01,9.450E-01,1.018E+00,1.091E+00,
    1.164E+00,1.236E+00,1.309E+00,1.382E+00,1.455E+00,
    1.527E+00,1.600E+00,1.673E+00,1.745E+00,1.818E+00,
    1.891E+00,1.964E+00,2.036E+00,2.109E+00,2.182E+00,
    2.255E+00,2.327E+00,2.400E+00,2.473E+00,2.545E+00,
    2.618E+00,2.691E+00,2.764E+00,2.836E+00,2.909E+00,
    2.982E+00,3.055E+00,3.127E+00,3.200E+00,3.273E+00,
    3.345E+00,3.418E+00,3.491E+00,3.564E+00,3.636E+00,
    3.709E+00,3.782E+00,3.855E+00,3.927E+00,4.000E+00
    },
    {
    1.0e-02,8.000E-02,1.600E-01,2.400E-01,3.200E-01,4.000E-01,
    4.800E-01,5.600E-01,6.400E-01,7.200E-01,8.000E-01,
    8.800E-01,9.600E-01,1.040E+00,1.120E+00,1.200E+00,
    1.280E+00,1.360E+00,1.440E+00,1.520E+00,1.600E+00,
    1.680E+00,1.760E+00,1.840E+00,1.920E+00,2.000E+00,
    2.080E+00,2.160E+00,2.240E+00,2.320E+00,2.400E+00,
    2.480E+00,2.560E+00,2.640E+00,2.720E+00,2.800E+00,
    2.880E+00,2.960E+00,3.040E+00,3.120E+00,3.200E+00,
    3.280E+00,3.360E+00,3.440E+00,3.520E+00,3.600E+00,
    3.680E+00,3.760E+00,3.840E+00,3.920E+00,4.000E+00,
    4.080E+00,4.160E+00,4.240E+00,4.320E+00,4.400E+00
    },
    {
    1.0e-02,8.700E-02,1.750E-01,2.620E-01,3.490E-01,4.360E-01,
    5.240E-01,6.110E-01,6.980E-01,7.850E-01,8.730E-01,
    9.600E-01,1.047E+00,1.135E+00,1.222E+00,1.309E+00,
    1.396E+00,1.484E+00,1.571E+00,1.658E+00,1.745E+00,
    1.833E+00,1.920E+00,2.007E+00,2.095E+00,2.182E+00,
    2.269E+00,2.356E+00,2.444E+00,2.531E+00,2.618E+00,
    2.705E+00,2.793E+00,2.880E+00,2.967E+00,3.055E+00,
    3.142E+00,3.229E+00,3.316E+00,3.404E+00,3.491E+00,
    3.578E+00,3.665E+00,3.753E+00,3.840E+00,3.927E+00,
    4.015E+00,4.102E+00,4.189E+00,4.276E+00,4.364E+00,
    4.451E+00,4.538E+00,4.625E+00,4.713E+00,4.800E+00
    },
    {
    1.0e-02,9.300E-02,1.850E-01,2.780E-01,3.710E-01,4.640E-01,
    5.560E-01,6.490E-01,7.420E-01,8.350E-01,9.270E-01,
    1.020E+00,1.113E+00,1.205E+00,1.298E+00,1.391E+00,
    1.484E+00,1.576E+00,1.669E+00,1.762E+00,1.855E+00,
    1.947E+00,2.040E+00,2.133E+00,2.225E+00,2.318E+00,
    2.411E+00,2.504E+00,2.596E+00,2.689E+00,2.782E+00,
    2.875E+00,2.967E+00,3.060E+00,3.153E+00,3.245E+00,
    3.338E+00,3.431E+00,3.524E+00,3.616E+00,3.709E+00,
    3.802E+00,3.895E+00,3.987E+00,4.080E+00,4.173E+00,
    4.265E+00,4.358E+00,4.451E+00,4.544E+00,4.636E+00,
    4.729E+00,4.822E+00,4.915E+00,5.007E+00,5.100E+00
    },
    {
    1.0e-02,9.800E-02,1.960E-01,2.950E-01,3.930E-01,4.910E-01,
    5.890E-01,6.870E-01,7.850E-01,8.840E-01,9.820E-01,
    1.080E+00,1.178E+00,1.276E+00,1.375E+00,1.473E+00,
    1.571E+00,1.669E+00,1.767E+00,1.865E+00,1.964E+00,
    2.062E+00,2.160E+00,2.258E+00,2.356E+00,2.455E+00,
    2.553E+00,2.651E+00,2.749E+00,2.847E+00,2.945E+00,
    3.044E+00,3.142E+00,3.240E+00,3.338E+00,3.436E+00,
    3.535E+00,3.633E+00,3.731E+00,3.829E+00,3.927E+00,
    4.025E+00,4.124E+00,4.222E+00,4.320E+00,4.418E+00,
    4.516E+00,4.615E+00,4.713E+00,4.811E+00,4.909E+00,
    5.007E+00,5.105E+00,5.204E+00,5.302E+00,5.400E+00
    },
    {
    1.0e-02,1.000E-01,2.000E-01,3.000E-01,4.000E-01,5.000E-01,
    6.000E-01,7.000E-01,8.000E-01,9.000E-01,1.000E+00,
    1.100E+00,1.200E+00,1.300E+00,1.400E+00,1.500E+00,
    1.600E+00,1.700E+00,1.800E+00,1.900E+00,2.000E+00,
    2.100E+00,2.200E+00,2.300E+00,2.400E+00,2.500E+00,
    2.600E+00,2.700E+00,2.800E+00,2.900E+00,3.000E+00,
    3.100E+00,3.200E+00,3.300E+00,3.400E+00,3.500E+00,
    3.600E+00,3.700E+00,3.800E+00,3.900E+00,4.000E+00,
    4.100E+00,4.200E+00,4.300E+00,4.400E+00,4.500E+00,
    4.600E+00,4.700E+00,4.800E+00,4.900E+00,5.000E+00,
    5.100E+00,5.200E+00,5.300E+00,5.400E+00,5.500E+00
    },
  };
  const double eProb[NEnergy][56]={
    {
    0.0,1.959E+00,1.763E+00,1.567E+00,1.404E+00,1.273E+00,
    1.110E+00,1.054E+00,1.121E+00,1.169E+00,1.197E+00,
    1.224E+00,1.260E+00,1.315E+00,1.399E+00,1.430E+00,
    1.463E+00,1.501E+00,1.534E+00,1.600E+00,1.632E+00,
    1.645E+00,1.658E+00,1.645E+00,1.616E+00,1.554E+00,
    1.506E+00,1.477E+00,1.466E+00,1.465E+00,1.338E+00,
    9.794E-01,8.619E-01,8.488E-01,8.477E-01,8.425E-01,
    8.349E-01,8.179E-01,7.992E-01,7.804E-01,7.674E-01,
    7.619E-01,7.656E-01,7.779E-01,7.709E-01,7.675E-01,
    7.610E-01,7.546E-01,7.470E-01,7.367E-01,5.876E-01,
    3.265E-01,1.175E-01,5.567E-02,2.205E-02,6.529E-03
    },
    {
    0.0,1.429E+01,1.353E+01,1.251E+01,1.123E+01,1.062E+01,
    1.021E+01,9.955E+00,9.699E+00,9.597E+00,9.589E+00,
    9.783E+00,1.006E+01,1.044E+01,1.100E+01,1.171E+01,
    1.184E+01,1.220E+01,1.268E+01,1.302E+01,1.312E+01,
    1.322E+01,1.337E+01,1.341E+01,1.323E+01,1.286E+01,
    1.242E+01,1.208E+01,1.188E+01,1.098E+01,1.021E+01,
    9.189E+00,8.168E+00,7.213E+00,7.203E+00,7.144E+00,
    7.058E+00,6.893E+00,6.799E+00,6.710E+00,6.534E+00,
    6.378E+00,6.278E+00,6.246E+00,6.215E+00,6.069E+00,
    5.954E+00,5.839E+00,5.703E+00,5.569E+00,5.105E+00,
    3.573E+00,1.787E+00,6.254E-01,3.710E-01,1.212E-01
    },
    {
    0.0,2.968E+01,2.661E+01,2.354E+01,2.047E+01,1.893E+01,
    1.763E+01,1.723E+01,1.818E+01,1.882E+01,1.918E+01,
    1.940E+01,1.965E+01,2.007E+01,2.073E+01,2.148E+01,
    2.254E+01,2.292E+01,2.344E+01,2.420E+01,2.436E+01,
    2.466E+01,2.497E+01,2.523E+01,2.527E+01,2.485E+01,
    2.426E+01,2.370E+01,2.327E+01,2.296E+01,2.282E+01,
    2.067E+01,1.617E+01,1.570E+01,1.560E+01,1.540E+01,
    1.511E+01,1.471E+01,1.422E+01,1.370E+01,1.324E+01,
    1.287E+01,1.259E+01,1.239E+01,1.191E+01,1.137E+01,
    1.085E+01,1.036E+01,9.868E+00,9.392E+00,8.187E+00,
    5.935E+00,2.763E+00,7.679E-01,3.203E-01,1.023E-01
    },
    {
    0.0,4.908E+01,4.330E+01,3.897E+01,3.609E+01,3.248E+01,
    2.959E+01,2.887E+01,2.815E+01,2.786E+01,2.743E+01,
    2.699E+01,2.663E+01,2.666E+01,2.683E+01,2.712E+01,
    2.747E+01,2.757E+01,2.743E+01,2.736E+01,2.930E+01,
    3.129E+01,3.118E+01,3.103E+01,3.157E+01,3.118E+01,
    3.073E+01,2.979E+01,2.886E+01,2.805E+01,2.747E+01,
    2.719E+01,2.714E+01,2.656E+01,2.598E+01,2.483E+01,
    2.396E+01,2.309E+01,2.237E+01,2.165E+01,2.093E+01,
    2.021E+01,1.905E+01,1.790E+01,1.588E+01,1.449E+01,
    1.368E+01,1.288E+01,1.212E+01,1.140E+01,1.069E+01,
    8.516E+00,3.176E+00,1.249E+00,7.245E-01,2.290E-01
    },
    {
    0.0,5.723E+01,5.186E+01,4.471E+01,3.845E+01,3.398E+01,
    3.040E+01,2.683E+01,2.628E+01,2.596E+01,2.827E+01,
    2.868E+01,2.868E+01,2.862E+01,2.860E+01,2.878E+01,
    2.872E+01,2.858E+01,2.858E+01,2.883E+01,2.844E+01,
    2.836E+01,2.981E+01,3.126E+01,3.040E+01,2.858E+01,
    2.814E+01,2.713E+01,2.590E+01,2.469E+01,2.359E+01,
    2.285E+01,2.057E+01,1.779E+01,1.783E+01,1.787E+01,
    1.781E+01,1.759E+01,1.721E+01,1.667E+01,1.606E+01,
    1.551E+01,1.529E+01,1.505E+01,1.474E+01,1.414E+01,
    1.344E+01,1.285E+01,1.220E+01,1.162E+01,1.105E+01,
    9.121E+00,6.617E+00,3.398E+00,1.019E+00,4.699E-01
    },
    {
    0.0,5.880E+01,5.488E+01,4.704E+01,4.312E+01,3.920E+01,
    3.724E+01,3.528E+01,3.430E+01,3.234E+01,2.930E+01,
    2.965E+01,2.957E+01,2.945E+01,2.937E+01,2.931E+01,
    2.928E+01,2.912E+01,2.897E+01,2.890E+01,2.895E+01,
    2.731E+01,2.755E+01,2.753E+01,2.697E+01,2.665E+01,
    2.626E+01,2.607E+01,2.587E+01,2.567E+01,2.548E+01,
    2.528E+01,2.509E+01,2.489E+01,2.450E+01,2.352E+01,
    2.254E+01,2.156E+01,2.058E+01,1.960E+01,1.862E+01,
    1.783E+01,1.703E+01,1.691E+01,1.678E+01,1.626E+01,
    1.570E+01,1.521E+01,1.473E+01,1.431E+01,1.385E+01,
    1.176E+01,7.839E+00,3.920E+00,1.215E+00,4.640E-01
    },
    {
    0.0,7.497E+01,6.560E+01,5.623E+01,5.061E+01,4.311E+01,
    3.842E+01,3.561E+01,3.215E+01,3.186E+01,3.168E+01,
    3.215E+01,3.228E+01,3.211E+01,3.192E+01,3.179E+01,
    3.160E+01,3.143E+01,3.127E+01,3.124E+01,3.127E+01,
    3.135E+01,2.955E+01,2.943E+01,2.943E+01,2.924E+01,
    2.924E+01,2.943E+01,2.961E+01,2.961E+01,2.980E+01,
    2.980E+01,2.961E+01,2.943E+01,2.905E+01,2.868E+01,
    2.793E+01,2.736E+01,2.680E+01,2.624E+01,2.568E+01,
    2.530E+01,2.487E+01,2.474E+01,2.451E+01,2.393E+01,
    2.344E+01,2.291E+01,2.256E+01,2.231E+01,2.099E+01,
    1.874E+01,1.387E+01,3.518E+00,7.964E-01,2.607E-01
    },
    {
    0.0,7.436E+01,6.543E+01,5.948E+01,5.353E+01,4.610E+01,
    4.164E+01,3.718E+01,3.272E+01,3.049E+01,2.828E+01,
    3.000E+01,3.046E+01,3.046E+01,3.029E+01,2.999E+01,
    2.964E+01,2.930E+01,2.901E+01,2.896E+01,2.922E+01,
    2.984E+01,3.093E+01,3.056E+01,3.182E+01,3.249E+01,
    3.282E+01,3.346E+01,3.495E+01,3.495E+01,3.307E+01,
    3.272E+01,3.197E+01,3.123E+01,3.049E+01,2.913E+01,
    2.618E+01,2.627E+01,2.632E+01,2.621E+01,2.600E+01,
    2.572E+01,2.535E+01,2.496E+01,2.460E+01,2.398E+01,
    2.283E+01,2.179E+01,2.081E+01,1.990E+01,1.901E+01,
    1.661E+01,1.263E+01,7.472E+00,2.867E+00,9.732E-01
    },
    {
    0.0,4.753E+01,4.086E+01,3.418E+01,3.085E+01,2.585E+01,
    2.335E+01,1.918E+01,1.834E+01,1.709E+01,1.668E+01,
    1.648E+01,1.661E+01,1.647E+01,1.621E+01,1.590E+01,
    1.562E+01,1.541E+01,1.533E+01,1.552E+01,1.613E+01,
    1.721E+01,1.874E+01,1.975E+01,2.162E+01,2.344E+01,
    2.478E+01,2.543E+01,2.501E+01,2.460E+01,2.335E+01,
    2.085E+01,1.834E+01,1.609E+01,1.505E+01,1.509E+01,
    1.399E+01,1.405E+01,1.399E+01,1.365E+01,1.315E+01,
    1.260E+01,1.214E+01,1.183E+01,1.164E+01,1.122E+01,
    1.076E+01,1.031E+01,9.837E+00,9.364E+00,8.891E+00,
    7.504E+00,5.420E+00,2.251E+00,6.935E-01,1.570E-01
    },
    {
    0.0,6.529E+00,5.659E+00,5.115E+00,4.353E+00,3.700E+00,
    3.265E+00,2.829E+00,2.503E+00,2.285E+00,2.231E+00,
    2.127E+00,2.139E+00,2.118E+00,2.081E+00,2.036E+00,
    1.998E+00,1.976E+00,1.977E+00,2.025E+00,2.135E+00,
    2.325E+00,2.591E+00,2.873E+00,3.136E+00,3.492E+00,
    3.738E+00,3.858E+00,3.609E+00,3.143E+00,2.782E+00,
    2.406E+00,2.099E+00,1.911E+00,1.841E+00,1.837E+00,
    1.757E+00,1.772E+00,1.761E+00,1.701E+00,1.612E+00,
    1.519E+00,1.442E+00,1.401E+00,1.394E+00,1.347E+00,
    1.305E+00,1.262E+00,1.210E+00,1.150E+00,1.089E+00,
    9.250E-01,6.638E-01,2.798E-01,9.130E-02,1.211E-02
    },
  };

  // ENERGY:
  SDef::SrcData D2(4);

  SDef::DSIndex DI4(std::string("dir"));
  for(size_t index=0;index<NEnergy;index++)
    {
      SDef::SrcInfo SI('h');
      SDef::SrcProb SP;  
      for(size_t i=0;i<sizeof(energy[index])/sizeof(double);i++)
	{
	  SI.addData(energy[index][i]);
	  SP.addData(eProb[index][i]);
	}
      DI4.addData(5+index,&SI,0,&SP);
    }
  D2.addUnit(&DI4);
  sourceCard.setData("erg",D2);    
  return;
}



void
LensSource::write(std::ostream& OX) const
  /*!
    Write out as a MCNP source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("LensSource","write");

  Source sourceCard;
  createSource(sourceCard);
  sourceCard.write(OX);
  return;
}

void
LensSource::writePHITS(std::ostream& OX) const
  /*!
    Write out as a PHITS source system
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("LensSource","write");

  ELog::EM<<"NOT YET WRITTEN "<<ELog::endCrit;
  return;
}
  
} // NAMESPACE SDef
