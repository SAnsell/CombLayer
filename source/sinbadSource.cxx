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
#include <boost/array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "DSTerm.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "ModelSupport.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "InsertComp.h"
#include "WorkData.h"
#include "sinbadSource.h"

using namespace std;
namespace SDef

{

sinbadSource::sinbadSource() : 
  cutEnergy(0.0),weight(1.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

sinbadSource::sinbadSource(const sinbadSource& A) : 
  cutEnergy(A.cutEnergy),weight(A.weight),
  CentPoint(A.CentPoint),Direction(A.Direction),
  angleSpread(A.angleSpread),radialSpread(A.radialSpread),
  Energy(A.Energy),EWeight(A.EWeight)
  /*!
    Copy constructor
    \param A :: sinbadSource to copy
  */
{}

sinbadSource&
sinbadSource::operator=(const sinbadSource& A)
  /*!
    Assignment operator
    \param A :: sinbadSource to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cutEnergy=A.cutEnergy;
      weight=A.weight;
      CentPoint=A.CentPoint;
      Direction=A.Direction;
      angleSpread=A.angleSpread;
      radialSpread=A.radialSpread;
      Energy=A.Energy;
      EWeight=A.EWeight;
    }
  return *this;
}

sinbadSource::~sinbadSource() 
  /*!
    Destructor
  */
{}

void
sinbadSource::createAll(SDef::Source& sourceCard) 
  /*!
    Generic function to create everything
    \param sourceCard :: Card to update
  */
{
  ELog::RegMethod RegA("sinbadSource","createAll");
  
  createSourceSinbad(sourceCard);
  return;
}
  
void
sinbadSource::createSourceSinbad(SDef::Source& sourceCard)
  /*!
    Creates a target 2 neutron source 
    for chipIR
    \param sourceCard :: Source system
  */
{
  ELog::RegMethod RegA("sinbadSource","createSource");
 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
     
  double sinbadSource49 [17] [16] = {
 { -49.75, -46.58, -43.32, -37.08, -33.92, -27.58, -11.75, -2.25,  7.25, 16.75, 32.58, 38.92, 42.08, 48.42, 51.58, 54.75 },
 {  51.44,    0  ,    0  ,    0  ,    0  ,    0  ,    0  , 2.935, 2.967, 2.861,   0  ,   0  ,   0  ,   0  ,   0  ,   0   },
 {  47.63,    0  ,    0  ,    0  ,    0  ,    0  ,  3.222, 3.529, 3.567, 3.446, 3.023,   0  ,   0  ,   0  ,   0  ,   0	 },
 {  40.64,    0  ,    0  ,    0  ,    0  ,  3.228,  3.782, 4.152, 4.208, 4.082, 3.615, 2.959,   0  ,   0  ,   0  ,   0	 },
 {  35.56,    0  ,    0  ,    0  ,  3.234,  3.566,  4.164, 4.575, 4.648, 4.523, 4.025, 3.317, 2.915,   0  ,   0  ,   0	 },
 {  31.75,    0  ,    0  ,  3.350,  3.745,  4.099,  4.750, 5.220, 5.325, 5.205, 4.658, 3.867, 3.423, 2.916,   0  ,   0	 },
 {  19.69,    0  ,  3.300,  3.756,  4.168,  4.538,  5.226, 5.743, 5.874, 5.760, 5.174, 4.315, 3.838, 3.305, 2.738,   0	 },
 {  15.88,  3.172,  3.522,  3.990,  4.412,  4.790,  5.499, 6.042, 6.186, 6.076, 5.466, 4.569, 4.073, 3.526, 2.951, 2.558 },
 {   5.29,  3.281,  3.641,  4.119,  4.546,  4.928,  5.645, 6.199, 6.350, 6.239, 5.613, 4.690, 4.184, 3.631, 3.056, 2.670 },
 {  -5.29,  3.145,  3.500,  3.967,  4.382,  4.750,  5.440, 5.972, 6.117, 6.003, 5.379, 4.466, 3.970, 3.433, 2.883, 2.519 },
 { -15.88,    0  ,  3.275,  3.721,  4.114,  4.461,  5.109, 5.611, 5.746, 5.632, 5.021, 4.132, 3.654, 3.142, 2.625,   0	 },
 { -19.69,    0  ,    0  ,  3.294,  3.645,  3.954,  4.535, 4.990, 5.113, 5.003, 4.421, 3.583, 3.139, 2.674,   0  ,   0	 },
 { -31.75,    0  ,    0  ,    0  ,  3.052,  3.317,  3.824, 4.229, 4.340, 4.236, 3.699, 2.937, 2.541,   0  ,   0  ,   0	 },
 { -35.56,    0  ,    0  ,    0  ,    0  ,  2.888,  3.359, 3.739, 3.841, 3.742, 3.244, 2.545,   0  ,   0  ,   0  ,   0	 },
 { -40.64,    0  ,    0  ,    0  ,    0  ,    0  ,  2.681, 3.036, 3.124, 3.032, 2.607,   0  ,   0  ,   0  ,   0  ,   0	 },
 { -47.63,    0  ,    0  ,    0  ,    0  ,    0  ,    0  , 2.399, 2.470, 2.382,   0  ,   0  ,   0  ,   0  ,   0  ,   0	 },
 { -51.44,    0  ,    0  ,    0  ,    0  ,    0  ,    0  , 0    , 0    , 0    ,   0  ,   0  ,   0  ,   0  ,   0  ,   0   } 
 } ; 

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // vector<double>  gridZ(17);
  // vector<double>  gridX(16);

  sourceCard.setActive();
 
  SDef::SrcData D1(1);
  SDef::SrcInfo SI1;
  SDef::SrcProb SP1;
  SI1.addData(1.4);
  SI1.addData(1.6);
  SP1.addData(0);
  SP1.addData(1.);
  D1.addUnit(SI1);  
  D1.addUnit(SP1);  
  sourceCard.setData("y",D1);  

  SDef::SrcData D2(2);
  SDef::SrcInfo SI2;
  SDef::SrcProb SP2;
  double hVal=0.0;
  SP2.addData(hVal);
  for(unsigned iz=0;iz<16;iz++) 
   {SI2.addData(sinbadSource49[16-iz][0]); 
    hVal=0; 
    for(unsigned ix=0;ix<15;ix++)hVal+=sinbadSource49[iz+1][ix+1]*(sinbadSource49[0][ix+1]-sinbadSource49[0][ix]);
    if(iz<15)SP2.addData(hVal);   
   }  
  D2.addUnit(SI2);  
  D2.addUnit(SP2);  
  sourceCard.setData("z",D2);  

  SDef::SrcData D3(3);
  SDef::SrcInfo SI3;
  for(unsigned ix=0;ix<15;ix++) SI3.addData(sinbadSource49[0][ix]-2.5); 
  SDef::DSTerm<int>* DS3=D3.getDS<int>(); 
  DS3->setType("z",'s');
  for (unsigned is=3;is<18;is++) DS3->addData(is);
  // sourceCard.setData("x",D3); 

 
  SDef::DSIndex DI4(std::string("z"));
  SDef::SrcInfo SI4;
  for(unsigned ix=0;ix<15;ix++) SI4.addData(sinbadSource49[0][ix]-2.5); 
  for (unsigned is=4;is<19;is++) 
   {
    SDef::SrcProb SP4;
    SP4.addData(0.0);
    for (unsigned ix=1;ix<15;ix++)SP4.addData(sinbadSource49[19-is][ix+1]*
                                             (sinbadSource49[0][ix+1]-sinbadSource49[0][ix]));
    DI4.addData(is,&SI4,0,&SP4);
   }
  D3.addUnit(&DI4);
  sourceCard.setData("x",D3); 

  return;
}  


} // NAMESPACE SDef
