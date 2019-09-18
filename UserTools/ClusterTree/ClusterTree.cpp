#include "ClusterTree.h"

ClusterTree::ClusterTree():Tool(){}


bool ClusterTree::Initialise(std::string configfile, DataModel &data)
{
  
  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();
  
  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////
  tf = new TFile("LAPPDAnalysis.root","RECREATE");
  fMyTree = new TTree("mytree","mytree");
  
  // set the branches
  
  fMyTree->Branch("NHits",            &NHits,             "NHits/I"               );
  fMyTree->Branch("Q",                hQ,                 "Q[NHits]/D"            );
  fMyTree->Branch("xpar",             hxpar,              "Xpar[NHits]/D"         ); 
  fMyTree->Branch("xperp",            hxperp,             "Xperp[NHits]/D"        );
  fMyTree->Branch("time",             htime,              "time[NHits]/D"         );
  fMyTree->Branch("deltime",          hdeltime,           "deltime[NHits]/D"      );
  fMyTree->Branch("Vpeak",            hvpeak,            "Vpeak[NHits]/D"        );
  
  
  
  return true;
}


bool ClusterTree::Execute()
{
  std::map<unsigned long,vector<LAPPDHit>> Hits;
  m_data->Stores["ANNIEEvent"]->Get("Cluster",Hits);
  map <unsigned long, vector<LAPPDHit>> :: iterator itr;
  vector<LAPPDHit> :: iterator itrr;
  
  
  for (itr = Hits.begin(); itr != Hits.end(); ++itr)
    {      
      unsigned long  channel= itr->first;
      int numberhits = itr->second.size();
      NHits = numberhits;
      int m=0;
      for (itrr = itr->second.begin(); itrr!= itr->second.end(); ++itrr)
	{
	  
	  hT[m]=itr->second.at(m).GetTime();
	  hQ[m]=itr->second.at(m).GetCharge();
	  hxpar[m]=itr->second.at(m).GetLocalPosition().at(0);
	  hxperp[m]=itr->second.at(m).GetLocalPosition().at(1);
	  htime[m]=itr->second.at(m).GetTime();
	}
      fMyTree->Fill();
    }
  fMyTree->Write();
  return true;
}


bool ClusterTree::Finalise(){
  
  // tf->Close();
  return true;
}
