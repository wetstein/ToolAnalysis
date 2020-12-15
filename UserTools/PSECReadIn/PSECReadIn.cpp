#include "PSECReadIn.h"

PSECReadIn::PSECReadIn():Tool(){}


bool PSECReadIn::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

  std::string NewFile;
  //get the data file name from the config file
  m_variables.Get("", NewFile)
  //Opening data file
  DataFile.open(NewFile, iso::in);
  std::cout<<"Opened file: "<<NewFile<<std::endl;
    
  //getting detais from the config file
  m_variables.Get("Nsamples", Nsamples);
  m_variables.Get("NChannels", NChannel);
  m_variables.Get("TrigChannel", TrigChannel);
    
  //since unsure if LAPPDFilter, LAPPDBaselineSubtract, or LAPPDcfd are being used, isFiltered, isBLsub, and isCFD are set to false
  bool isFiltered = false;
  m_data->Stores["ANNIEEvent"]->Header->Set("isFiltered",isFiltered);
  bool isBLsub = false;
  m_data->Stores["ANNIEEvent"]->Header->Set("isBLsubtracted",isBLsub);
  bool isCFD=false;
  m_data->Stores["ANNIEEvent"]->Header->Set("isCFD",isCFD);

  return true;
}


bool PSECReadIn::Execute(){

  return true;
}


bool PSECReadIn::Finalise(){

  return true;
}
