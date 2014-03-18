#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Element.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "sinbadMaterial.h"

namespace ModelSupport
{

void addSinbadMaterial()
  /*!
     Initialize the database of materials
   */
{
  ELog::RegMethod RegA("sinbadMaterial","addSinbadMaterial");

  // Get Main database
  DBMaterial& DBObj=DBMaterial::Instance();
 
  const std::string MLib="hlib=.70h pnlib=70u";

  MonteCarlo::Material MObj;
  MObj.setMaterial(101,"sbadMildSteel",  
		   "6000.70c 8.523759e-02 5010.70c "
		   "1.894100e-08 5011.70c 7.576401e-08",
		   "",MLib);
  MObj.setDensity(8.5217e-02);
  DBObj.setMaterial(MObj);


  MObj.setMaterial(102,"sbadLead",  
		   " 82204.71c  4.6183E-04 82206.71c  7.9501E-03 "
		   " 82207.71c  7.2904E-03 82208.71c  1.7286E-02 ",
		   "",MLib);
  MObj.setDensity(3.2988e-02);
  DBObj.setMaterial(MObj);

  MObj.setMaterial(103,"sbadSulphurSteel",  
		   "  6000.00c  1.6983E-04 15031.71c  1.6983E-05 "
		   " 16032.71c  2.4183E-05 16033.71c  1.9361E-07 "
		   " 16034.71c  1.0929E-06 16036.71c  5.0949E-09 "
		   " 25055.71c  1.9530E-04 26054.71c  4.9394E-03 "
		   " 26056.71c  7.7539E-02 26057.71c  1.7907E-03 "
		   " 26058.71c  2.3831E-04 ",
		   "",MLib);
  MObj.setDensity(8.4915e-02);
  DBObj.setMaterial(MObj);

  MObj.setMaterial(104,"Rh","45103.71c  7.2625e-02 ",
		   "",MLib);
  DBObj.setMaterial(MObj);

  MObj.setMaterial(105,"Gold",
		   " 79197.71c  5.9009E-02 ",
		   "",MLib);
  DBObj.setMaterial(MObj);

     //M103 Rh


  return;
}

} // NAMESPACE ModelSupport
