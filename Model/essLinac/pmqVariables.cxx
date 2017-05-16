// This file is generated by the 'optics2var' script from the optics file.
// Namespace(dtl='LinacDTL1', end='D204US', opt='optics.dat', start='quad1213')

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

namespace setVariable
{
void
EssLinacPMQVariables(FuncDataBase &Control)
  /*!
    Create all the PMQ variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("essVariables[F]","EssLinacPMQVariables");

  Control.addVariable("LinacDTL1PMQ1Length",5);
  Control.addVariable("LinacDTL1PMQ1GapLength",10.2267);
  Control.addVariable("LinacDTL1PMQ2Length",5);
  Control.addVariable("LinacDTL1PMQ2GapLength",10.7922);
  Control.addVariable("LinacDTL1PMQ3Length",5);
  Control.addVariable("LinacDTL1PMQ3GapLength",11.4001);
  Control.addVariable("LinacDTL1PMQ4Length",5);
  Control.addVariable("LinacDTL1PMQ4GapLength",12.0186);
  Control.addVariable("LinacDTL1PMQ5Length",5);
  Control.addVariable("LinacDTL1PMQ5GapLength",12.6467);
  Control.addVariable("LinacDTL1PMQ6Length",5);
  Control.addVariable("LinacDTL1PMQ6GapLength",13.2801);
  Control.addVariable("LinacDTL1PMQ7Length",5);
  Control.addVariable("LinacDTL1PMQ7GapLength",13.9227);
  Control.addVariable("LinacDTL1PMQ8Length",5);
  Control.addVariable("LinacDTL1PMQ8GapLength",14.5767);
  Control.addVariable("LinacDTL1PMQ9Length",5);
  Control.addVariable("LinacDTL1PMQ9GapLength",15.2378);
  Control.addVariable("LinacDTL1PMQ10Length",5);
  Control.addVariable("LinacDTL1PMQ10GapLength",15.9004);
  Control.addVariable("LinacDTL1PMQ11Length",5);
  Control.addVariable("LinacDTL1PMQ11GapLength",16.5756);
  Control.addVariable("LinacDTL1PMQ12Length",5);
  Control.addVariable("LinacDTL1PMQ12GapLength",17.2519);
  Control.addVariable("LinacDTL1PMQ13Length",5);
  Control.addVariable("LinacDTL1PMQ13GapLength",17.9386);
  Control.addVariable("LinacDTL1PMQ14Length",5);
  Control.addVariable("LinacDTL1PMQ14GapLength",18.6271);
  Control.addVariable("LinacDTL1PMQ15Length",5);
  Control.addVariable("LinacDTL1PMQ15GapLength",19.3196);
  Control.addVariable("LinacDTL1PMQ16Length",5);
  Control.addVariable("LinacDTL1PMQ16GapLength",20.0176);
  Control.addVariable("LinacDTL1PMQ17Length",5);
  Control.addVariable("LinacDTL1PMQ17GapLength",20.7118);
  Control.addVariable("LinacDTL1PMQ18Length",5);
  Control.addVariable("LinacDTL1PMQ18GapLength",21.4204);
  Control.addVariable("LinacDTL1PMQ19Length",5);
  Control.addVariable("LinacDTL1PMQ19GapLength",22.1234);
  Control.addVariable("LinacDTL1PMQ20Length",5);
  Control.addVariable("LinacDTL1PMQ20GapLength",22.8328);
  Control.addVariable("LinacDTL1PMQ21Length",5);
  Control.addVariable("LinacDTL1PMQ21GapLength",23.5462);
  Control.addVariable("LinacDTL1PMQ22Length",5);
  Control.addVariable("LinacDTL1PMQ22GapLength",24.256);
  Control.addVariable("LinacDTL1PMQ23Length",5);
  Control.addVariable("LinacDTL1PMQ23GapLength",24.968);
  Control.addVariable("LinacDTL1PMQ24Length",5);
  Control.addVariable("LinacDTL1PMQ24GapLength",25.6916);
  Control.addVariable("LinacDTL1PMQ25Length",5);
  Control.addVariable("LinacDTL1PMQ25GapLength",26.4079);
  Control.addVariable("LinacDTL1PMQ26Length",5);
  Control.addVariable("LinacDTL1PMQ26GapLength",27.1268);
  Control.addVariable("LinacDTL1PMQ27Length",5);
  Control.addVariable("LinacDTL1PMQ27GapLength",27.8447);
  Control.addVariable("LinacDTL1PMQ28Length",5);
  Control.addVariable("LinacDTL1PMQ28GapLength",28.5674);
  Control.addVariable("LinacDTL1PMQ29Length",5);
  Control.addVariable("LinacDTL1PMQ29GapLength",29.2496);
  Control.addVariable("LinacDTL1PMQ30Length",5);
  Control.addVariable("LinacDTL1PMQ30GapLength",29.6263);
  Control.addVariable("LinacDTL1PMQ31Length",5);
  Control.addVariable("LinacDTL1PMQ31GapLength",1);
  Control.addVariable("LinacDTL1NPMQ",31);

  Control.addVariable("LinacDTL2PMQ1Length",5);
  Control.addVariable("LinacDTL2PMQ1GapLength",29.908);
  Control.addVariable("LinacDTL2PMQ2Length",8);
  Control.addVariable("LinacDTL2PMQ2GapLength",28.7645);
  Control.addVariable("LinacDTL2PMQ3Length",8);
  Control.addVariable("LinacDTL2PMQ3GapLength",29.4515);
  Control.addVariable("LinacDTL2PMQ4Length",8);
  Control.addVariable("LinacDTL2PMQ4GapLength",30.1815);
  Control.addVariable("LinacDTL2PMQ5Length",8);
  Control.addVariable("LinacDTL2PMQ5GapLength",30.9092);
  Control.addVariable("LinacDTL2PMQ6Length",8);
  Control.addVariable("LinacDTL2PMQ6GapLength",31.6348);
  Control.addVariable("LinacDTL2PMQ7Length",8);
  Control.addVariable("LinacDTL2PMQ7GapLength",32.3564);
  Control.addVariable("LinacDTL2PMQ8Length",8);
  Control.addVariable("LinacDTL2PMQ8GapLength",33.0747);
  Control.addVariable("LinacDTL2PMQ9Length",8);
  Control.addVariable("LinacDTL2PMQ9GapLength",33.7895);
  Control.addVariable("LinacDTL2PMQ10Length",8);
  Control.addVariable("LinacDTL2PMQ10GapLength",34.5005);
  Control.addVariable("LinacDTL2PMQ11Length",8);
  Control.addVariable("LinacDTL2PMQ11GapLength",35.2071);
  Control.addVariable("LinacDTL2PMQ12Length",8);
  Control.addVariable("LinacDTL2PMQ12GapLength",35.9112);
  Control.addVariable("LinacDTL2PMQ13Length",8);
  Control.addVariable("LinacDTL2PMQ13GapLength",36.6108);
  Control.addVariable("LinacDTL2PMQ14Length",8);
  Control.addVariable("LinacDTL2PMQ14GapLength",37.3062);
  Control.addVariable("LinacDTL2PMQ15Length",8);
  Control.addVariable("LinacDTL2PMQ15GapLength",37.9972);
  Control.addVariable("LinacDTL2PMQ16Length",8);
  Control.addVariable("LinacDTL2PMQ16GapLength",38.3937);
  Control.addVariable("LinacDTL2PMQ17Length",8);
  Control.addVariable("LinacDTL2PMQ17GapLength",39.6163);
  Control.addVariable("LinacDTL2PMQ18Length",8);
  Control.addVariable("LinacDTL2PMQ18GapLength",1);
  Control.addVariable("LinacDTL2NPMQ",18);

  Control.addVariable("LinacDTL3PMQ1Length",8);
  Control.addVariable("LinacDTL3PMQ1GapLength",40.5869);
  Control.addVariable("LinacDTL3PMQ2Length",8);
  Control.addVariable("LinacDTL3PMQ2GapLength",40.9382);
  Control.addVariable("LinacDTL3PMQ3Length",8);
  Control.addVariable("LinacDTL3PMQ3GapLength",41.5867);
  Control.addVariable("LinacDTL3PMQ4Length",8);
  Control.addVariable("LinacDTL3PMQ4GapLength",42.2312);
  Control.addVariable("LinacDTL3PMQ5Length",8);
  Control.addVariable("LinacDTL3PMQ5GapLength",42.8709);
  Control.addVariable("LinacDTL3PMQ6Length",8);
  Control.addVariable("LinacDTL3PMQ6GapLength",43.5064);
  Control.addVariable("LinacDTL3PMQ7Length",8);
  Control.addVariable("LinacDTL3PMQ7GapLength",44.1372);
  Control.addVariable("LinacDTL3PMQ8Length",8);
  Control.addVariable("LinacDTL3PMQ8GapLength",44.7635);
  Control.addVariable("LinacDTL3PMQ9Length",8);
  Control.addVariable("LinacDTL3PMQ9GapLength",45.3853);
  Control.addVariable("LinacDTL3PMQ10Length",8);
  Control.addVariable("LinacDTL3PMQ10GapLength",46.0014);
  Control.addVariable("LinacDTL3PMQ11Length",8);
  Control.addVariable("LinacDTL3PMQ11GapLength",46.615);
  Control.addVariable("LinacDTL3PMQ12Length",8);
  Control.addVariable("LinacDTL3PMQ12GapLength",47.2241);
  Control.addVariable("LinacDTL3PMQ13Length",8);
  Control.addVariable("LinacDTL3PMQ13GapLength",47.4799);
  Control.addVariable("LinacDTL3PMQ14Length",8);
  Control.addVariable("LinacDTL3PMQ14GapLength",48.7397);
  Control.addVariable("LinacDTL3PMQ15Length",8);
  Control.addVariable("LinacDTL3PMQ15GapLength",49.0898);
  Control.addVariable("LinacDTL3PMQ16Length",8);
  Control.addVariable("LinacDTL3PMQ16GapLength",1);
  Control.addVariable("LinacDTL3NPMQ",16);

  Control.addVariable("LinacDTL4PMQ1Length",8);
  Control.addVariable("LinacDTL4PMQ1GapLength",49.8069);
  Control.addVariable("LinacDTL4PMQ2Length",8);
  Control.addVariable("LinacDTL4PMQ2GapLength",50.3783);
  Control.addVariable("LinacDTL4PMQ3Length",8);
  Control.addVariable("LinacDTL4PMQ3GapLength",50.9451);
  Control.addVariable("LinacDTL4PMQ4Length",8);
  Control.addVariable("LinacDTL4PMQ4GapLength",51.5073);
  Control.addVariable("LinacDTL4PMQ5Length",8);
  Control.addVariable("LinacDTL4PMQ5GapLength",52.0652);
  Control.addVariable("LinacDTL4PMQ6Length",8);
  Control.addVariable("LinacDTL4PMQ6GapLength",52.6186);
  Control.addVariable("LinacDTL4PMQ7Length",8);
  Control.addVariable("LinacDTL4PMQ7GapLength",53.1663);
  Control.addVariable("LinacDTL4PMQ8Length",8);
  Control.addVariable("LinacDTL4PMQ8GapLength",53.713);
  Control.addVariable("LinacDTL4PMQ9Length",8);
  Control.addVariable("LinacDTL4PMQ9GapLength",54.2508);
  Control.addVariable("LinacDTL4PMQ10Length",8);
  Control.addVariable("LinacDTL4PMQ10GapLength",54.7437);
  Control.addVariable("LinacDTL4PMQ11Length",8);
  Control.addVariable("LinacDTL4PMQ11GapLength",54.7173);
  Control.addVariable("LinacDTL4PMQ12Length",8);
  Control.addVariable("LinacDTL4PMQ12GapLength",56.3914);
  Control.addVariable("LinacDTL4PMQ13Length",8);
  Control.addVariable("LinacDTL4PMQ13GapLength",56.7879);
  Control.addVariable("LinacDTL4PMQ14Length",8);
  Control.addVariable("LinacDTL4PMQ14GapLength",1);
  Control.addVariable("LinacDTL4NPMQ",14);

  Control.addVariable("LinacDTL5PMQ1Length",8);
  Control.addVariable("LinacDTL5PMQ1GapLength",57.1164);
  Control.addVariable("LinacDTL5PMQ2Length",8);
  Control.addVariable("LinacDTL5PMQ2GapLength",57.6422);
  Control.addVariable("LinacDTL5PMQ3Length",8);
  Control.addVariable("LinacDTL5PMQ3GapLength",58.1658);
  Control.addVariable("LinacDTL5PMQ4Length",8);
  Control.addVariable("LinacDTL5PMQ4GapLength",58.6849);
  Control.addVariable("LinacDTL5PMQ5Length",8);
  Control.addVariable("LinacDTL5PMQ5GapLength",59.1972);
  Control.addVariable("LinacDTL5PMQ6Length",8);
  Control.addVariable("LinacDTL5PMQ6GapLength",59.7063);
  Control.addVariable("LinacDTL5PMQ7Length",8);
  Control.addVariable("LinacDTL5PMQ7GapLength",60.2108);
  Control.addVariable("LinacDTL5PMQ8Length",8);
  Control.addVariable("LinacDTL5PMQ8GapLength",60.1365);
  Control.addVariable("LinacDTL5PMQ9Length",8);
  Control.addVariable("LinacDTL5PMQ9GapLength",60.2081);
  Control.addVariable("LinacDTL5PMQ10Length",8);
  Control.addVariable("LinacDTL5PMQ10GapLength",72.3022);
  Control.addVariable("LinacDTL5PMQ11Length",25);
  Control.addVariable("LinacDTL5PMQ11GapLength",58);
  Control.addVariable("LinacDTL5PMQ12Length",25);
  Control.addVariable("LinacDTL5PMQ12GapLength",1);
  Control.addVariable("LinacDTL5NPMQ",12);

  return;
}
}
    
