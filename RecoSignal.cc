
#include<RecoSignal.h>

ClassImp(RecoSignal)

//==========================================================================
RecoSignal::RecoSignal(gate::VLEVEL vl, std::string label) : 
IAlgo(vl,"RecoSignal",0,label){
//==========================================================================
  
  this->setDefaults();

}

//==========================================================================
RecoSignal::RecoSignal(const gate::ParamStore& gs, 
		       gate::VLEVEL vl, std::string label) :
  IAlgo(gs,vl,"RecoSignal",0,label){
//==========================================================================
  
  this->setDefaults();

}

//==========================================================================
void RecoSignal::setDefaults(){
//==========================================================================
  
  _minS1Hits = 1;

  _minS2Hits = 1;

  _minS2width = 4*gate::microsecond;

  _maxS1width = 3*gate::microsecond;

  _nSigOverPedSiPM = 3;

  //! initialize counters

  _nS1hmap = 0; _nS2hmap = 0; 

}

//==========================================================================
bool RecoSignal::initialize(){
//==========================================================================

  _m.message("Intializing algorithm",this->getAlgoLabel(),gate::NORMAL);
  
  this->bookHistos();

  return true;

}

//==========================================================================
void RecoSignal::bookHistos(){
//==========================================================================
  
  gate::HistoManager* hman = gate::Centella::instance()->hman();
  
  hman->h1("nHit","nHit; PMT hits in signals; Entries ",20,0,20);

  hman->h1("nHitS1","nHit; PMT hits in S1 signals; Entries ",20,0,20);
  
  hman->h1("nHitS2","nHit; PMT hits in S2 signals; Entries ",20,0,20);
  
  hman->h1("nSiHitS1","nSiHit; SiPM hits in S1 signals; Entries ",256,0,256);
  
  hman->h1("nSiHitS2","nSiHit; SiPM hits in S2 signals; Entries ",256,0,256);

  hman->h1("sT","sT; Signal Start time (#mus); Entries ",800,0,800);
  
  hman->h1("wT","wT; Signal width (#mus); Entries ",100,0,50);

  hman->h1("sTS1","sT; S1 Signal Start time (#mus); Entries ",800,0,800);
  
  hman->h1("wTS1","wT; S1 Signal width (#mus); Entries ",100,0,50);
  
  hman->h1("sTS2","sT; S2 Signal Start time (#mus); Entries ",800,0,800);
  
  hman->h1("wTS2","wT; S2 Signal width (#mus); Entries ",100,0,50);

  hman->h1("nSigEvt","nSigEvt; Signals per Event; Entries ",10,0,10);

  hman->h1("nS1Evt","nS1Evt; S1s per Event; Entries ",10,0,10);

  hman->h1("nS2Evt","nS2Evt; S2s per Event; Entries ",10,0,10);

  hman->h1("ES1","S1 Energy; S1 Energy (PE); Entries ",110,0,1100);

  hman->h1("ES2","S2 Energy; S2 Energy (PE); Entries ",110,0,42000);

}

//==========================================================================
bool RecoSignal::execute(gate::Event& evt){
//==========================================================================

  _m.message("Executing algorithm",this->getAlgoLabel(),gate::VERBOSE);
  
  _m.message("Event number:",evt.GetEventID(),gate::VERBOSE);
  
  _nS1Evt = 0; _nS2Evt = 0;

  std::vector<gate::Hit*> pmts = evt.GetHits(gate::PMT);
  
  std::vector<gate::Hit*>::iterator ih;
  
  for (ih = pmts.begin(); ih != pmts.end(); ++ih){
       
    const std::vector<gate::Pulse*>& pulses = (*ih)->GetWaveform().GetPulses();
    
    std::vector<gate::Pulse*>::const_iterator ip;
    
    for (ip = pulses.begin(); ip != pulses.end(); ++ip){

      if ((*ip)->find_istore("InSignal")) continue;

      iSignal signal;

      signal.pulses.push_back( *ip );

      signal.hits.push_back( *ih );

      signal.sT = (*ip)->GetStartTime(); 

      signal.eT = (*ip)->GetEndTime(); 

      signal.amp = (*ip)->GetAmplitude(); 

      std::vector<gate::Hit*>::iterator ih2;
      
      for (ih2 = ih+1; ih2 != pmts.end(); ++ih2){
	 
	std::vector<gate::Pulse*> pulses2 = (*ih2)->GetWaveform().GetPulses();
	
	 std::vector<gate::Pulse*>::iterator ip2;
	 
	 for (ip2 = pulses2.begin(); ip2 != pulses2.end(); ++ip2){
	   
	   if ((*ip2)->find_istore("InSignal")) continue;

	   double ts2 = (*ip2)->GetStartTime(); 

	   double te2 = (*ip2)->GetEndTime(); 
	   
	   if (ts2 > signal.eT) break; // all other pulses come later

	   if( (ts2 > signal.sT && ts2< signal.eT) || 
	       
	       (te2 > signal.sT && te2< signal.eT)){ //same signal
	     
	     signal.pulses.push_back(*ip2); 
	     
	     signal.hits.push_back(*ih2); 

	     signal.sT = std::min(ts2,signal.sT); 

	     signal.eT = std::max(te2,signal.eT);
	     
	     signal.amp += (*ip2)->GetAmplitude(); 

	     (*ip2)->store("InSignal",1);
	   } 

	 } // end of 2nd loop over pulses
      
      }// end of 2nd loop over channels

      _m.message("Number of pulses in signal:",
		 
		 signal.pulses.size(),gate::VERBOSE);
      
      _m.message("Time window:",signal.sT,"-",signal.eT,gate::VERBOSE);
      
      this->buildGateSignal(signal,evt);
      
    } // end of 1st loop over pulses
  
  } // end of 1st loop over channels
  

  this->fillEvtHistos();

  return true;

}

//==========================================================================
bool RecoSignal::finalize(){
//==========================================================================

  _m.message("Finalising algorithm",this->getAlgoLabel(),gate::NORMAL);
  
  _m.message("Number of S1 hitmaps:",_nS1hmap,gate::NORMAL);
  
  _m.message("Number of S2 hitmaps:",_nS2hmap,gate::NORMAL);
    
  size_t ntot = gate::Centella::instance()->getInEvents(); 

  _m.message("S1 signals per event:",_nS1hmap*1./ntot,gate::NORMAL);

  _m.message("S2 signals per event:",_nS2hmap*1./ntot,gate::NORMAL);
  
  gate::Centella::instance()->hman()->draw("nHit");
  
  gate::Centella::instance()->hman()->draw("nHitS1");
  
  gate::Centella::instance()->hman()->draw("nHitS2");
  
  gate::Centella::instance()->hman()->draw("nSiHitS1");
  
  gate::Centella::instance()->hman()->draw("nSiHitS2");

  gate::Centella::instance()->hman()->draw("sT");
  
  gate::Centella::instance()->hman()->draw("sTS1");

  gate::Centella::instance()->hman()->draw("sTS2");
  
  gate::Centella::instance()->hman()->draw("wT");

  gate::Centella::instance()->hman()->draw("wTS1");
  
  gate::Centella::instance()->hman()->draw("wTS2");
  
  gate::Centella::instance()->hman()->draw("nSigEvt");

  gate::Centella::instance()->hman()->draw("nS1Evt");

  gate::Centella::instance()->hman()->draw("nS2Evt");

  gate::Centella::instance()->hman()->draw("ES1");

  gate::Centella::instance()->hman()->draw("ES2");

  return true;

}


//==========================================================================
void RecoSignal::fillSigHistos(gate::Signal* sig){
//==========================================================================
  
  double unit = gate::microsecond;
  
  gate::SIGNALTYPE type = sig->GetSignalType();

  size_t nhits = sig->GetCatHitMap().GetMap(0).size(); 
    
  size_t nsihits = 0;

  std::vector<std::map<int,double> >::const_iterator im;

  std::vector<std::map<int,double> > hm = sig->GetAnoHitMap().GetTimeMap();

  for (im = hm.begin(); im != hm.end(); ++im){nsihits += (*im).size();}
  
  if (nsihits) nsihits /= hm.size(); //average

  double width = sig->GetEndTime() - sig->GetStartTime();
  
  double startT = sig->GetStartTime();

  gate::Centella::instance()->hman()->fill("nHit",nhits);
  
  gate::Centella::instance()->hman()->fill("sT",startT/unit);
  
  gate::Centella::instance()-> hman()->fill("wT",width/unit);
  
  if (type==gate::S1) { 
    
    gate::Centella::instance()->hman()->fill("nHitS1",nhits);
    
    gate::Centella::instance()->hman()->fill("nSiHitS1",nsihits);

    gate::Centella::instance()->hman()->fill("sTS1",startT/unit);
  
    gate::Centella::instance()-> hman()->fill("wTS1",width/unit);
    
    gate::Centella::instance()->hman()->fill("ES1",sig->GetAmplitude());

  }
  
  else if (type==gate::S2){
    
    gate::Centella::instance()->hman()->fill("nHitS2",nhits);
    
    gate::Centella::instance()->hman()->fill("nSiHitS2",nsihits);

    gate::Centella::instance()->hman()->fill("sTS2",startT/unit);
  
    gate::Centella::instance()-> hman()->fill("wTS2",width/unit);
    
    gate::Centella::instance()->hman()->fill("ES2",sig->GetAmplitude());

  }

}


//==========================================================================
void RecoSignal::fillEvtHistos(){
//==========================================================================
   
  gate::Centella::instance()->hman()->fill("nSigEvt",_nS1Evt+_nS2Evt);
  
  gate::Centella::instance()->hman()->fill("nS1Evt",_nS1Evt);

  gate::Centella::instance()->hman()->fill("nS2Evt",_nS2Evt);

}


//==========================================================================
void RecoSignal::buildGateSignal(iSignal sig, gate::Event& evt){
//==========================================================================
  
  gate::SIGNALTYPE stype = this->signalType(sig);
  
  if (!this->isGoodSignal(sig,stype)) return; // not very elegant

  //------- Create cathode hitmap ------//

  gate::HitMap* chmap = new gate::HitMap();

  evt.AddHitMap(gate::PMT,chmap);

  chmap->SetSensorType(gate::PMT);

  chmap->SetStartTime(sig.sT);
  
  chmap->SetEndTime(sig.eT);
  
  chmap->SetAmplitude(sig.amp);
  
  chmap->SetTimeSample(sig.hits[0]->GetWaveform().GetSampWidth());
  
  chmap->SetSignalType(stype);
  
  std::vector<std::map<int,double> > cmap;
  
  std::vector<gate::Pulse*>::iterator ip;
  
  std::map<int,double> tcmap;
  
  for (ip =sig.pulses.begin(); ip != sig.pulses.end(); ++ip){
    
    tcmap[(*ip)->GetSensorID()] = (*ip)->GetAmplitude(); }
  
  cmap.push_back(tcmap); // just one time slice (integrated)
  
  chmap->SetTimeMap(cmap);
  
  //------- Create anode hitmap ------//
  
  gate::HitMap* ahmap = new gate::HitMap();

  evt.AddHitMap(gate::SIPM,ahmap);
  
  ahmap->SetSensorType(gate::SIPM);

  ahmap->SetStartTime(sig.sT);
  
  ahmap->SetEndTime(sig.eT);

  std::vector<gate::Hit*> sipms = evt.GetHits(gate::SIPM);
  
  double swidth = sipms[0]->GetWaveform().GetSampWidth();

  int isample = (int) sig.sT / swidth;

  int fsample = (int) sig.eT / swidth;
  
  std::vector<std::map<int,double> > amap;
  
  for (int i=isample; i<fsample; i++){
    
    std::map<int,double> tamap;

    std::vector<gate::Hit*>::iterator ih;
 
    for (ih = sipms.begin(); ih != sipms.end(); ++ih){
      
      double amp = (*ih)->GetWaveform().GetAmplitude(i);
      
      double ped = (*ih)->GetWaveform().GetBaseline();
      
      double pedRMS = (*ih)->GetWaveform().GetBaselineRMS();
      
      double minAmp =  pedRMS * _nSigOverPedSiPM;

      amp -= ped; 

      if( amp > minAmp) tamap[(*ih)->GetSensorID()] = amp;
      
    }
        
    amap.push_back(tamap);
  }   
  
  ahmap->SetTimeMap(amap);

  //------- Create gate Signal ------//
  
  gate::Signal* gsig = new gate::Signal();
  
  evt.AddSignal(gsig);
  
  gsig->SetCatHitMap(chmap); 

  gsig->SetAnoHitMap(ahmap);
  
  gsig->SetStartTime(sig.sT);

  gsig->SetEndTime(sig.eT);
  
  gsig->SetAmplitude(sig.amp);

  gsig->SetSignalType(stype);

  this->fillSigHistos(gsig);

  //------- Increase counters ------//

  if (stype==gate::S1){ _nS1hmap += 1;  _nS1Evt += 1;}

  else { _nS2hmap += 1; _nS2Evt += 1;}

  return;

}

//==========================================================================
bool RecoSignal::isGoodSignal(iSignal signal, gate::SIGNALTYPE stype){
//==========================================================================
  
  //! TO BE FURTHER DEVELOPED!!!!!

  bool ok = true;

  if (stype==gate::S1){ ok = (signal.pulses.size()>_minS1Hits); }
  
  else { ok = (signal.pulses.size()>_minS2Hits); }

  return ok;
  
}

//==========================================================================
gate::SIGNALTYPE RecoSignal::signalType(iSignal signal){
//==========================================================================
  
  gate::SIGNALTYPE type;
  
  double width = signal.eT-signal.sT;

  if ( width < _maxS1width ){ type = gate::S1; } 

  else if ( width > _minS2width ){ type = gate::S2; } 
  
  //!! TO BE DEVELOPED!!!!!!!

  return type;

}
