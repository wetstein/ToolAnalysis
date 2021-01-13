#include "LAPPDReorderData.h"

LAPPDReorderData::LAPPDReorderData():Tool(){}


bool LAPPDReorderData::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

    TString IWL,OWL;
    m_variables.Get("ReorderInputWavLabel",IWL);
    InputWavLabel = IWL;
    m_variables.Get("ReorderOutputWavLabel",OWL);
    OutputWavLabel = OWL;

    m_variables.Get("DelayOffset",delayoffset);



  return true;
}


bool LAPPDReorderData::Execute(){

    vector<string> acdcmetadata;
    m_data->Stores["ANNIEEvent"]->Get("ACDCmetadata",acdcmetadata);
    std::map<unsigned long,vector<Waveform<double>>> lappddata;
    m_data->Stores["ANNIEEvent"]->Get(InputWavLabel,lappddata);
    std::map<unsigned long,vector<Waveform<double>>> reordereddata;

    // Loop over waveforms, reorder data

    string Smeta26_1 = acdcmetadata.at(52);
    string Smeta26_2 = acdcmetadata.at(53);
    int meta26_1 = stoi(Smeta26_1, 0, 16);
    int meta26_2 = stoi(Smeta26_2, 0, 16);

    //cout<<"REORDER TIME!!!!   "<<acdcmetadata.size()<<" "<<acdcmetadata.at(52)<<" "<<acdcmetadata.at(53)<<" "<<meta26_1<<" "<<meta26_2<<endl;

    map <unsigned long, vector<Waveform<double>>> :: iterator itr;

    for (itr = lappddata.begin(); itr != lappddata.end(); ++itr){
      unsigned long channelno = itr->first;
      vector<Waveform<double>> Vwavs = itr->second;


        int switchbit=0;
        if(channelno<30) switchbit = meta26_1*32;
        else switchbit = meta26_2*32;

        switchbit+=delayoffset;

        // cout<<"channel= "<<channelno<<endl;

        vector<Waveform<double>> Vrwav;

        //loop over all Waveforms
        for(int i=0; i<Vwavs.size(); i++){

            Waveform<double> bwav = Vwavs.at(i);
            Waveform<double> rwav;


            for(int j=0; j< bwav.GetSamples()->size(); j++){

                if(switchbit>255 || switchbit<0) switchbit=0;
                double nsamp = bwav.GetSamples()->at(switchbit);
                rwav.PushSample(nsamp);
                switchbit++;

            }

            Vrwav.push_back(rwav);
        }

        reordereddata.insert(pair<unsigned long, vector<Waveform<double>>>(1000+channelno,Vrwav));

    }


    m_data->Stores["ANNIEEvent"]->Set(OutputWavLabel,reordereddata);


  return true;
}


bool LAPPDReorderData::Finalise(){

  return true;
}
