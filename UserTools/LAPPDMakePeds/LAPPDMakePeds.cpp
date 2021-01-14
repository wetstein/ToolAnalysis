#include "LAPPDMakePeds.h"

LAPPDMakePeds::LAPPDMakePeds():Tool(){}


bool LAPPDMakePeds::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

  m_data->Stores["ANNIEEvent"]->Header->Get("AnnieGeometry", _geom);

  TString IWL;
  //RawInputWavLabel;
  m_variables.Get("PedWavLabel",IWL);
  InputWavLabel = IWL;

  m_variables.Get("NChannels",NChannels);

  pedhists = new std::map<unsigned long, TH1D>;


  return true;
}


bool LAPPDMakePeds::Execute(){

  // get raw lappd data
  std::map<unsigned long,vector<Waveform<double>>> lappddata;

  cout<<"In MakePeds Execute"<<endl;

  //m_data->Stores["ANNIEEvent"]->Get("RawLAPPDData",rawlappddata);
  m_data->Stores["ANNIEEvent"]->Get(InputWavLabel,lappddata);

  // loop over all channels
  std::map<unsigned long, vector<Waveform<double>>> :: iterator itr;
  for (itr = lappddata.begin(); itr != lappddata.end(); ++itr){

      unsigned long channelNo = itr->first;
      vector<Waveform<double>> Vwavs = itr->second;

      cout<<"Looping, on channel: "<<(int)channelNo<<endl;

      Channel* mychannel = _geom->GetChannel(channelNo);
      //figure out the stripline number
      int stripno = mychannel->GetStripNum();
      //figure out the side of the stripline
      int stripside = mychannel->GetStripSide();

      cout<<"here"<<endl;

      if((pedhists->count(channelNo))==0){
        TString hname;
        hname+="pedch";
        hname+=channelNo;
        TH1D thepeds(hname,hname,1000,0,1000);
        cout<<"new TH1"<<endl;
        pedhists->insert(pair<unsigned long, TH1D>(channelNo,thepeds));
      }

      cout<<"here 2"<<endl;

      for(int i=0; i<Vwavs.size(); i++){
          Waveform<double> bwav = Vwavs.at(i);

          int nbins = bwav.GetSamples()->size();

          for(int i=0; i<nbins; i++){
            double sampleval = bwav.GetSamples()->at(i);
            ((pedhists->find(channelNo))->second).Fill(sampleval);
            cout<<i<<" "<<((pedhists->find(channelNo))->second).GetMean()<<endl;
          }
        }
      }


  return true;
}


bool LAPPDMakePeds::Finalise(){

  cout<<"Woohoo"<<endl;

/*
  std::map<unsigned long, TH1D> :: iterator itr;
  for (itr = pedhists.begin(); itr != pedhists.end(); ++itr){
    cout<<"avg ped value="<<(itr->second).GetMean()<<endl;
  }

  pedhists.clear();
  */
  return true;
}
