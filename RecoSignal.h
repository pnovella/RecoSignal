#ifndef _RecoSignal__
#define _RecoSignal__

#include <GATE/Centella.h>

//typedef std::vector<gate::Pulse*> Signal;

struct Signal{
  
  double sT;
  double eT;
  double amp;
  std::vector<gate::Pulse*> pulses;
  std::vector<gate::Hit*> hits;

};

class RecoSignal : public gate::IAlgo {
  
 public:
  
  //! default contructor
  RecoSignal(gate::VLEVEL=gate::NORMAL,
	       std::string label="RecoSignalInstance");
  
  //! constructor with store with input parameters 
  RecoSignal(const gate::ParamStore& gs,
	       gate::VLEVEL=gate::NORMAL,
	       std::string label="RecoSignalInstance");
  
  //! destructor
  virtual ~RecoSignal(){};
  
  //! initialize algorithm
  bool initialize();        
  
  //! execute algorithm: process current event
  bool execute(gate::Event& evt);  
  
  //! finalize algorithm
  bool finalize();          
  
 public:
  
  //! set min number of hits for a good signal
  inline void SetMinHits(size_t n){ _minHits = n; }

  //! set max width for a S1 signal
  inline void SetMaxS1width(double w){ _maxS1width = w; }

  //! set min width for a S2 signal
  inline void SetMinS2width(double w){ _minS2width = w; }

 private:
  
  //! true if good signal 
  bool isGoodSignal(Signal signal);
  
  //! get signal type 
  gate::SIGNALTYPE signalType(Signal signal);

  //! set default parameters
  void setDefaults();  
  
  //! book histograms
  void bookHistos();
  
  //! fill signal histograms
  void fillSigHistos(Signal sig);
  
  //! fill event histograms
  void fillEvtHistos(const gate::Event& evt);

  //! create gate HitMap from signal
  gate::HitMap* buildHitMap(Signal sig);
  
 private:
  
  //! min number of hits for a good signal
  size_t _minHits;
  
  //! min width of S2 signals
  double _minS2width;
    
  //! max width of S1 signals
  double _maxS1width;
  
  //! counter for S1 hitmaps
  size_t _nS1hmap; 
  
  //! counter for S2 hitmaps
  size_t _nS2hmap;

  ClassDef(RecoSignal,0)
    
};


#endif
