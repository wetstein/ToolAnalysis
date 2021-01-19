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

  pedhists = new std::map<unsigned long, vector<TH1D>>;
  //pedrootfile = new TFile("PedHist.root","RECREATE");

  return true;
}


bool LAPPDMakePeds::Execute(){
  bool firstevent = false;
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
      cout<< "Vwavs size "<< Vwavs.size()<<endl;
      cout<<"Looping, on channel: "<<(int)channelNo<<endl;

      Channel* mychannel = _geom->GetChannel(channelNo);
      //figure out the stripline number
      int stripno = mychannel->GetStripNum();
      //figure out the side of the stripline
      int stripside = mychannel->GetStripSide();

    //  cout<<"here"<<endl;
    if((pedhists->count(channelNo))==0)
    {
      firstevent=true;
      cout<<"FIRST EVENT ON CHANNEL"<<endl;
    }


    //  cout<<"here 2"<<endl;

      for(int i=0; i<Vwavs.size(); i++)//number of waves? how many?
      {
        if(firstevent)
        {

        //  TH1D thepeds(hname,hname,1000,0,1000);
          std::vector<TH1D> histvect;
          //cout<<"new vector"<<endl;
      //  pedhists= new map<unsigned long, vector<TH1D>>(channelNo,thepeds);
          pedhists->insert (std::pair<unsigned long, vector<TH1D>>(channelNo,histvect));
        }
          Waveform<double> bwav = Vwavs.at(i);
          int nbins = bwav.GetSamples()->size();

          for(int j=0; j<nbins; j++)// number of samples 256?
          {
            double sampleval = bwav.GetSamples()->at(j);
            if(firstevent)
            {
              TString hname;
              hname+="pedch_";
              hname+=channelNo;
              hname+="_";
              hname+=i;
              hname+="_";
              hname+= j;
              TH1D thepeds(hname,hname,1000,0,1000);
            //  cout<<"new TH1D"<<endl;
              (pedhists->find(channelNo))->second.push_back(thepeds);
            }
            (pedhists->find(channelNo))->second.at(j).Fill(sampleval);
            cout<<"sample added_"<<pedhists->find(channelNo)->second.at(j).GetMean()<< endl;
          }

        }
      }


  return true;
}


bool LAPPDMakePeds::Finalise(){

  string a, b, fileName1, fileName2;
  vector<string> datavalues1;
  vector<string> datavalues2;
  cout << "ENTER FILE 1 NAME";
  cin >> fileName1;
  cout << "ENTER FILE 2 NAME";
  cin >> fileName2;
  fileName1 = fileName1 + ".txt";
  fileName2 = fileName2 + ".txt";
  ofstream txtOut;
  txtOut.open (fileName1);
  bool firstchannel=true;
  std::map<unsigned long, vector<TH1D>> :: iterator itr;
  for (itr = pedhists->begin(); itr != pedhists->end(); ++itr)
  {
    unsigned long channelno = itr-> first;
    vector<TH1D> hists = itr->second;
    int nhists = hists.size();
    cout<<"CHANNEL NUMBER"<<channelno<<endl;
    if (channelno==30)
    {
      firstchannel=true;
    }
    for(int i=0; i<nhists; i++)
    {
      TF1 *f1 = new TF1("f1","gaus",0,1000);
      double max = itr->second.at(i).GetMaximum();
      double mean =  itr->second.at(i).GetMean();
      double rms =  itr->second.at(i).GetRMS();
      f1->SetParameters(max,mean,rms);
      hists.at(i).Fit("f1","Q");
      //cout<<"avg ped value= "<<(itr->second).at(i).GetMean()<<endl;
      //cout<<"avg ped value with gaussian"<<f1->GetParameters()[1]<<endl;
      //pedrootfile->cd();
      //hists.at(i).Write();
      double mu = f1->GetParameters()[1];
      string mutext = std::to_string(mu);
      if (channelno<30)
      {
        if (firstchannel)
        {
          string line;
          line =mutext;
          datavalues1.push_back(line);
        }
        else
        {
          string templine =datavalues1.at(i);
          templine = templine +" " + mutext;
          datavalues1.at(i)=templine;
        }
      }
      else
      {
        if (firstchannel)
        {
          string line;
          line =mutext;
          datavalues2.push_back(line);
        }
        else
        {
          string templine =datavalues2.at(i);
          templine = templine +" " + mutext;
          datavalues2.at(i)=templine;
        }
      }
    }
    firstchannel=false;
  }
  cout<<datavalues1.size()<<endl;
  cout<<datavalues2.size()<<endl;
  for (int j=0; j<datavalues1.size(); j++)
  {
    txtOut << datavalues1.at(j)<<endl;
  }
  txtOut.close();
  txtOut.open(fileName2);
  for (int j=0; j<datavalues2.size(); j++)
  {
    txtOut << datavalues2.at(j)<<endl;
  }
//  pedhists->clear();
  txtOut.close();
//  pedrootfile->Close();
  return true;
}
