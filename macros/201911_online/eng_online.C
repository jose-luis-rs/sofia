 /*  
 *  Author: Jose Luis <joseluis.rodriguez.sanchez@usc.es>
 *  @since October 20, 2019
 * 
 */

typedef struct EXT_STR_h101_t {
  EXT_STR_h101_unpack_t unpack;
  //EXT_STR_h101_MUSIC_onion_t music;
  //EXT_STR_h101_SOFTWIM_onion_t twim;
  //EXT_STR_h101_SOFMWPC_onion_t mwpc;
} EXT_STR_h101;

void at_online() {
  TStopwatch timer;
  timer.Start();
  
  const Int_t nev = -1; // number of events to read, -1 - until CTRL+C
  //const Int_t nev = 10; // Only 10 events to read  

  // Create source using ucesb for input ------------------
  
  TString filename = "--stream=lxg0898:6002";

  TString outputFileName = "data_at_online.root";
  
  TString ntuple_options = "RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  TString ucesb_path = ucesb_dir + "/../upexps/201911_eng/201911_eng";
  ucesb_path.ReplaceAll("//","/");
  
  EXT_STR_h101 ucesb_struct;
/*
  R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
					      ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
  source->SetMaxEvents(nev);


  // Definition of reader --------------------------------- 
  R3BUnpackReader* unpackreader = new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
					offsetof(EXT_STR_h101, unpack));

  // Add readers ------------------------------------------ 
  source->AddReader(unpackreader);


  // Create online run ------------------------------------ 
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(1);
  Int_t refresh = 1;
  Int_t port=8044;
  run->ActivateHttpServer(refresh, port);


  // Runtime data base ------------------------------------ 
  FairRuntimeDb* rtdb = run->GetRuntimeDb();


  // Initialize ------------------------------------------- 
  run->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");


  // Run -------------------------------------------------- 
  run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);


*/

  // Finish ----------------------------------------------- 
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  std::cout << std::endl << std::endl;
  std::cout << "Macro finished succesfully." << std::endl;
  std::cout << "Output file is " << outputFileName << std::endl;
  std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
            << std::endl << std::endl;
  //gApplication->Terminate();
}
