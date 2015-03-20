#ifndef _RecoSignal__
#define _RecoSignal__

#include <GATE/Centella.h>

struct iSignal{
  
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
  
  //! set min number of hits for a good S1 signal
  inline void SetMinS1Hits(size_t n){ _minS1Hits = n; }
  
  //! set min number of hits for a good S2 signal
  inline void SetMinS2Hits(size_t n){ _minS2Hits = n; }

  //! set max width for a S1 signal
  inline void SetMaxS1width(double w){ _maxS1width = w; }

  //! set min width for a S2 signal
  inline void SetMinS2width(double w){ _minS2width = w; }

  //! set numbe of sigmas over baseline to define a good SiPM pulse
  inline void SetNsigOverPedSiPM(double n){ _nSigOverPedSiPM = n; }

 private:
  
  //! true if good signal 
  bool isGoodSignal(iSignal signal,gate::SIGNALTYPE stype);
  
  //! get signal type 
  gate::SIGNALTYPE signalType(iSignal signal);

  //! set default parameters
  void setDefaults();  
  
  //! book histograms
  void bookHistos();
  
  //! fill signal histograms
  void fillSigHistos(gate::Signal* sig);
  
  //! fill event histograms
  void fillEvtHistos();

  //! create gate HitMap from signal
  void buildGateSignal(iSignal sig, gate::Event& evt);
  
 private:
  
  //! min number of hits for a good S1 signal
  size_t _minS1Hits;

  //! min number of hits for a good S2 signal
  size_t _minS2Hits;
  
  //! min width of S2 signals
  double _minS2width;
    
  //! max width of S1 signals
  double _maxS1width;
  
  //! number of sigmas over baseline defining a good SiPM pulse
  double _nSigOverPedSiPM; 

  //! counter for total S1 hitmaps
  size_t _nS1hmap; 
  
  //! counter for total S2 hitmaps
  size_t _nS2hmap;
  
  //! counter for number of S1s in event 
  size_t _nS1Evt;
  
  //! counter for number of S2s in event 
  size_t _nS2Evt;

  ClassDef(RecoSignal,0)
    
};


#endif
