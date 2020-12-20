#include "PSECReadIn.h"

PSECReadIn::PSECReadIn():Tool(){}


bool PSECReadIn::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

  string NewFile;
  //get the data file name from the config file
  //needs to have a path to the data files (inside the "")
  m_variables.Get("PSECinputfile", NewFile);
  //Opening data file
  DataFile.open(NewFile);
    
  if(!DataFile.is_open())
  {
      cout<<"Failed to open "<<NewFile<<"!"<<endl;
      return false;
  }
    
  cout<<"Opened file: "<<NewFile<<endl;
    
  //getting detais from the config file
  m_variables.Get("Nsamples", Nsamples);
  m_variables.Get("NChannels", NChannels);
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
  //create raw lappd object
  LAPPDWaveforms = new std::map<unsigned long, vector<Waveform<double>>>;
    
  //create a waveform for temp storage
  int eventNo=0; //event number
  
  string nextLine; //temp line to parse
    
  map <unsigned long, vector<Waveform<double>>> :: iterator itr;
    
  while(getline(DataFile, nextLine))
  {
      int sampleNo=0; //sample number
      unsigned long channelNo=0; //channel number
      istringstream iss(nextLine); //copies the current line in the file
      int location=-1; //counts the current perameter in the line
      string stempValue; //current string in the line
      int tempValue; //current int in the line

      //starts the loop at the beginning of the line
      while(iss >> stempValue)
      {
          location++;
          //  cout<<location<<endl;
          //when location==0, it is at the sample number
          
          //cout<<"beginning "<<stempValue<<endl;
          
          if(location==0)
          {
              sampleNo = stoi(stempValue, 0, 16);
              
              //(int)strtol(tempValue, NULL, 16);
              //cout<<"SAMPLE: "<<sampleNo<<endl;
              //sampleNo=tempValue;
              continue;
          }
          //this is the meta data per format
          if((location)%31==0)
          {
              //cout<<"yes"<<endl;
              continue;
          }
          
          int tempValue = stoi(stempValue, 0, 10);

          
          if(sampleNo==0)
          {
              Waveform<double> tempwav;
              //if none of the above if statements happen
              //it is going through each channel 0 through the NChannels-1
              //the tempValue will be placed into the tempwav
              tempwav.PushSample((double)tempValue);
              //cout<<"does it get here? "<<location<<" "<<tempValue<<endl;
              vector<Waveform<double>> Vtempwav;
              Vtempwav.push_back(tempwav);
              //each tempwav will be inserted into the correct channelno
              LAPPDWaveforms->insert(pair<unsigned long, vector<Waveform<double>>>(channelNo,Vtempwav));
              //clear the tempwav for a new value
              //cout<<"FIRST SAMPLE: "<<eventNo<<" "<<sampleNo<<" "<<channelNo<<" "<<tempValue<<endl;
          }
          else
          {
              //fitr = LAPPDWaveforms->find(channelNo);
              (((LAPPDWaveforms->find(channelNo))->second).at(0)).PushSample((double)tempValue);
              //cout<<"LATE SAMPLES: "<<eventNo<<" "<<sampleNo<<" "<<channelNo<<" "<<tempValue<<endl;
              //cout<<"Nsamples "<<(((LAPPDWaveforms->find(channelNo))->second).at(0)).Samples().size()<<endl;
              //vector<Waveform<double>> Vtemp = itr->second;
              //((itr->second).at(0)).PushSample(tempValue);
              //LAPPDWaveforms->insert(pair<unsigned long, vector<Waveform<double>>>(channelNo, Vtemp));
          }
        
        channelNo++;
      }
      
      //when the sampleNo gets to ff
      //it is at sample 256 and needs to go to the next event
      //if(sampleNo=='ff')
      if(sampleNo==255)
      {
          cout<<"DID THIS HAPPEN "<<sampleNo<<" "<<eventNo<<endl;
          eventNo++;
          break;
      }
      
      //cout<<"at the end"<<endl;
  }
    
  m_data->Stores["ANNIEEvent"]->Set("LAPPDWaveforms",LAPPDWaveforms);
  LAPPDWaveforms->clear();

  return true;
}


bool PSECReadIn::Finalise(){
  DataFile.close();
  return true;
}
