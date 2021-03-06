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
  rsig->SetMinS1Hits(4);
  rsig->SetMinS2Hits(14);

  gate::Centella::instance(gate::NORMAL);
  gate::Centella::instance()->addInputFile("/home/pnovella/Physics/NEXT/DATA/DST_3350_000_RP.root");
  gate::Centella::instance()->addOutputFile("test.root");
  gate::Centella::instance()->setNevents(100);
  gate::Centella::instance()->saveEvents(true);
  gate::Centella::instance()->saveHistos(true);
  gate::Centella::instance()->addAlgo("my_algo1",rsig);
    
  gate::Centella::instance()->run();
  
  gate::Centella::instance()->destroy();
  delete rsig;

}
