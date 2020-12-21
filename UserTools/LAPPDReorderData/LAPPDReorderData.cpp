#include "LAPPDReorderData.h"

LAPPDReorderData::LAPPDReorderData():Tool(){}


bool LAPPDReorderData::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

  return true;
}


bool LAPPDReorderData::Execute(){

  return true;
}


bool LAPPDReorderData::Finalise(){

  return true;
}
