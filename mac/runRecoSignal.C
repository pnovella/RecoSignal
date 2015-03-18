{
  
  gSystem->Load("libCore.so");
  gSystem->Load("libRIO.so");
  gSystem->Load("libHist.so");
  gSystem->Load("libGpad.so");

  gSystem->Load("$(GATE_DIR)/lib/libGATE.so");
  gSystem->Load("$(GATE_DIR)/lib/libGATEIO.so");
  gSystem->Load("$(GATE_DIR)/lib/libGATEUtils.so");

  gSystem->Load("$(GATE_DIR)/lib/libRecoSignal.so");

  RecoSignal* rsig = new RecoSignal(gate::NORMAL,"algo1");
  rsig->SetMinHits(14);

  gate::Centella::instance(gate::NORMAL);
  gate::Centella::instance()->addInputFile("../../../DATA/DST_Cs137_3345_000_RP.root");
  gate::Centella::instance()->addOutputFile("output_dst.root");
  gate::Centella::instance()->setNevents(10000);
  gate::Centella::instance()->saveEvents(false);
  gate::Centella::instance()->saveHistos(true);
  gate::Centella::instance()->addAlgo("my_algo1",rsig);
    
  gate::Centella::instance()->run();
  
  gate::Centella::instance()->destroy();
  delete rsig;

}
