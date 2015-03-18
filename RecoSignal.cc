
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
  
  _minHits = 1;

  _minS2width = 4*gate::microsecond;

  _maxS1width = 3*gate::microsecond;
  
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
  
  hman->h1("nHit","nHit; Number of hits in signal; Entries ",20,0,20);
  
  hman->h1("sT","sT; Signal Start time (#mus); Entries ",800,0,800);
  
  hman->h1("wT","wT; Signal width (#mus); Entries ",100,0,50);

  hman->h1("nSigEvt","nSigEvt; Signals per Event; Entries ",10,0,10);

}

//==========================================================================
bool RecoSignal::execute(gate::Event& evt){
//==========================================================================

  _m.message("Executing algorithm",this->getAlgoLabel(),gate::VERBOSE);
  
  _m.message("Event number:",evt.GetEventID(),gate::VERBOSE);
  
  std::vector<gate::Hit*> pmts = evt.GetHits(gate::PMT);
  
  std::vector<gate::Hit*>::iterator ih;
  
  for (ih = pmts.begin(); ih != pmts.end(); ++ih){
       
    const std::vector<gate::Pulse*>& pulses = (*ih)->GetWaveform().GetPulses();
    
    std::vector<gate::Pulse*>::const_iterator ip;
    
    for (ip = pulses.begin(); ip != pulses.end(); ++ip){

      if ((*ip)->find_istore("InSignal")) continue;

      Signal signal;

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
      
      if ( this->isGoodSignal(signal) ){
	
	this->fillSigHistos(signal);
	
	gate::HitMap* hmap = this->buildHitMap(signal);
	
	evt.AddHitMap(gate::PMT,hmap);
	
      }

    } // end of 1st loop over pulses
  
  } // end of 1st loop over channels
  

  this->fillEvtHistos(evt);

  return true;

}

//==========================================================================
bool RecoSignal::finalize(){
//==========================================================================

  _m.message("Finalising algorithm",this->getAlgoLabel(),gate::NORMAL);
  
  _m.message("Number of S1 hitmaps:",_nS1hmap,gate::NORMAL);
  
  _m.message("Number of S2 hitmaps:",_nS1hmap,gate::NORMAL);

  gate::Centella::instance()->hman()->draw("nHit");

  gate::Centella::instance()->hman()->draw("sT");

  gate::Centella::instance()->hman()->draw("wT");
  
  gate::Centella::instance()->hman()->draw("nSigEvt");

  return true;

}

//==========================================================================
void RecoSignal::fillSigHistos(Signal sig){
//==========================================================================
  
  double unit = gate::microsecond;

  gate::Centella::instance()->hman()->fill("nHit",sig.pulses.size());
      
  gate::Centella::instance()->hman()->fill("sT",sig.sT/unit);

  gate::Centella::instance()-> hman()->fill("wT",(sig.eT-sig.sT)/unit);
  
}

//==========================================================================
void RecoSignal::fillEvtHistos(const gate::Event& evt){
//==========================================================================
    
  size_t nSigs = evt.GetHitMaps(gate::PMT).size();

  gate::Centella::instance()->hman()->fill("nSigEvt", nSigs);
        
}


//==========================================================================
gate::HitMap* RecoSignal::buildHitMap(Signal sig){
//==========================================================================
  
  gate::HitMap* hmap = new gate::HitMap();
  
  hmap->SetSensorType(gate::PMT);

  hmap->SetStartTime(sig.sT);
  
  hmap->SetEndTime(sig.eT);
  
  hmap->SetAmplitude(sig.amp);
  
  hmap->SetTimeSample(sig.hits[0]->GetWaveform().GetSampWidth());
  
  gate::SIGNALTYPE stype = this->signalType(sig);

  hmap->SetSignalType(stype);

  //! TODO: add map of hits!!!!

  //! TODO: create corresponding SiPM hit map
    
  if (stype==gate::S1) _nS1hmap += 1;

  else _nS2hmap += 1;

  return hmap;

}

//==========================================================================
bool RecoSignal::isGoodSignal(Signal signal){
//==========================================================================

  bool ok = (signal.pulses.size()>_minHits);

  return ok;
  
}

//==========================================================================
gate::SIGNALTYPE RecoSignal::signalType(Signal signal){
//==========================================================================
  
  gate::SIGNALTYPE type;
  
  double width = signal.eT-signal.sT;

  if ( width < _maxS1width ){ type = gate::S1; } 

  else if ( width > _minS2width ){ type = gate::S2; } 
  
  //!! TO BE DEVELOPED!!!!!!!

  return type;

}
