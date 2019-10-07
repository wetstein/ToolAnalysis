#include "LAPPDCluster.h"

LAPPDCluster::LAPPDCluster():Tool(),_geom(nullptr){}


bool LAPPDCluster::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////
  //Geometry* _geom = new Geometry();
  m_data->Stores["ANNIEEvent"]->Header->Get("AnnieGeometry", _geom);


  return true;
}


bool LAPPDCluster::Execute(){

  cout<<"executing lappdcluster!!!"<<endl;

  std::map <unsigned long, vector<LAPPDPulse>> SimpleRecoLAPPDPulses;
  m_data->Stores["ANNIEEvent"]->Get("SimpleRecoLAPPDPulses",SimpleRecoLAPPDPulses);
  std::map <unsigned long, vector<LAPPDPulse>> :: iterator pulseitr;


  cout<<"Grabbed RecoLAPPDPulses "<<SimpleRecoLAPPDPulses.size()<<endl;

  vector<unsigned long> chanhand;
  for (pulseitr = SimpleRecoLAPPDPulses.begin(); pulseitr != SimpleRecoLAPPDPulses.end(); ++pulseitr){


    unsigned long chankey = pulseitr->first;
    bool handled= false;
    for(int i=0; i<chanhand.size(); ++i)
      {

        unsigned long key = chanhand[i];
        if(chankey==key)
        {
          handled= true;
        }
      }
      if (handled)
      {
        break;
      }
    cout<<"pulseitr chankey is "<<chankey<<endl;

    vector<LAPPDPulse> vPulse = pulseitr->second;

    cout<<"iterating!!!   "<<chankey<<" "<<vPulse.size()<<endl;

    if(vPulse.size()>1) {cout<<"VPULSE HAS A SIZE OF: "<<vPulse.size()<<endl;}
    for(int jj=0; jj<vPulse.size(); jj++){
      LAPPDPulse apulse = vPulse.at(jj);
      cout<<"the charge of this pulse is: "<<apulse.GetCharge()<<endl;
    }

    Channel* mychannel= _geom->GetChannel(chankey);
    cout<<"the strip number is :"<<mychannel->GetStripNum()<<" and the side is: "<<mychannel->GetStripSide()<<endl;


    std::map <unsigned long, vector<LAPPDPulse>> :: iterator oppoitr;

    for (oppoitr = SimpleRecoLAPPDPulses.begin(); oppoitr != SimpleRecoLAPPDPulses.end(); ++oppoitr){

      unsigned long oppochankey = oppoitr->first;

      cout<<"oppochankey is "<<oppochankey<<endl;

      vector<LAPPDPulse> oppovPulse = oppoitr->second;
      Channel* oppochannel = _geom->GetChannel(oppochankey);
      int mystripnum = mychannel->GetStripNum();
      int oppostripnum = oppochannel->GetStripNum();

      cout<<"mystripnum is "<<mystripnum<<endl;
      cout<<"oppostripnum is "<<oppostripnum<<endl;

      if( (oppochankey != chankey) && (mystripnum == oppostripnum) ){
        cout<<"channel "<<chankey<<" and "<<oppochankey<<" are on the same strip."<<endl;

        chanhand.push_back(oppochankey);
      }
      if ( (oppochankey != chankey) && (std::abs(oppostripnum-mystripnum)==1) ){
        if (mychannel->GetStripSide() == oppochannel->GetStripSide()){
          cout<<"channel "<<chankey<<" and "<<oppochankey<<" are on the same sides of adjacent strips."<<endl;

          chanhand.push_back(oppochankey);
        }
        else {
          cout<<"channel "<<chankey<<" and "<<oppochankey<<" are on the opposite sides of adjacent strips."<<endl;

          chanhand.push_back(oppochankey);
        }
      }






    //  SimpleRecoLAPPDPulses.erase(chankey);
    //  SimpleRecoLAPPDPulses.erase(oppochankey);
    }
    chanhand.push_back(chankey);
    cout<< "HANDLED" << endl;
  }

  return true;
}


bool LAPPDCluster::Finalise(){

  return true;
}
