#ifndef _RecoSignal__
#define _RecoSignal__

#include <GATE/Centella.h>

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
  
 private:
  
  ClassDef(RecoSignal,0)
    
};

#endif
