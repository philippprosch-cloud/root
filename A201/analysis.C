#define analysis_cxx
#include "analysis.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TRint.h>
#include <iostream>
#include <math.h>

int renum(int wire){
  if (wire % 2 == 1){
  return wire + 2;
  } 
  return wire;
}

void analysis::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L analysis.C
//      Root > analysis t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   TH1D* driftTimesHisto = new TH1D("Driftzeiten", "Driftzeiten", 251, -2.5/2., 250*2.5+2.5/2.);
   TH2 *wireCorrHisto = new TH2D("wireCorrelation","Drahtkorrelation: gefiltert und umnummeriert",
				 48,0.5+1,48.5+1, 48, 0.5+1, 48.5+1);
   TH2 *wiredriftHisto = new TH2D("wire_drift", "Drahtnummer gegen Driftzeit: gefiltert",
				 48,0.5,48.5, 251, -2.5/2., 250*2.5+2.5/2.);
   TH2 *driftTotHisto = new TH2D("drift_tot", "Driftzeit gegen TOT: gefiltert",
				 251, -2.5/2., 250*2.5+2.5/2., 235, -2.5/2., 235*2.5 - 2.5/2);  
  
  TH1D* wireHisto = new TH1D("wire", "Drahtnummern", 48, 0.5+1, 48.5+1);

  
   TH1D* dzh = &*driftTimesHisto;
   TH1D* odb = new TH1D("odb", "Orts-Driftzeitbeziehung", 251, -2.5/2., 250*2.5+2.5/2.);

   // assumes m = 20
   TH1D* alphaHisto = new TH1D("alpha", "Winkelverteilung", 251, -0.9189, 1.0687);

  
  Long64_t tot_threshold = 140 ; // in ns
  
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();


   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry); nbytes += nb;
      
      for(UInt_t hit=0; hit<nhits_le; hit++) {
	  Double_t time=time_le[hit]*2.5;


    Double_t tot_sec=tot[hit]*2.5;
    if (tot_sec>tot_threshold){
	  driftTimesHisto->Fill(time);
    wireHisto->Fill(renum(wire_le[hit]));

    double alpha = atan((renum(wire_le[hit]) - 20)*0.85/13.3);
    alphaHisto->Fill(alpha);
      }


    Double_t sum = 0;
    for (UInt_t bin = 1; bin <= dzh->GetNbinsX(); ++bin){
    sum += dzh->GetBinContent(bin);
    odb->SetBinContent(bin,sum);
    }
    odb->Scale(8.5/sum);


	for (UInt_t j=0; j<nhits_le; j++) {
	  if (hit==j) {
	    continue;}
          Double_t tot_sec=tot[j]*2.5; //converts tot to ns
          if (tot[j]*2.5 > tot_threshold && tot[hit]*2.5 > tot_threshold){
            wireCorrHisto->Fill(renum(wire_le[hit]), renum(wire_le[j]));
            //wireCorrHisto->Fill(wire_le[hit], wire_le[j]);
          }
	}

	for (UInt_t j=0; j<nhits_le; j++) {

          Double_t tot_sec=tot[j]*2.5; //converts tot to ns
          Double_t time=time_le[j]*2.5;

          if (tot[j]*2.5 > tot_threshold && tot[hit]*2.5 > tot_threshold){ 
            // driftzeit vs drahtnummer
            wiredriftHisto->Fill(renum(wire_le[hit]),time);

            // driftzeit/tot
            driftTotHisto->Fill(time, tot_sec);
        }

	}
      }
            
      // if (Cut(ientry) < 0) continue;
   }
   driftTimesHisto->GetXaxis()->SetTitle("Zeit / ns");
   driftTimesHisto->GetYaxis()->SetTitle("Trefferanzahl");
   //gStyle->SetOptStat(0);
   //driftTimesHisto->Draw();
   wireCorrHisto->GetXaxis()->SetTitle("Drahtnummer");
   wireCorrHisto->GetYaxis()->SetTitle("Drahtnummer");
   //wireCorrHisto->Draw("colz");

   // meine histogramme :)
   wiredriftHisto->GetXaxis()->SetTitle("Drahtnummer");
   wiredriftHisto->GetYaxis()->SetTitle("Driftzeit / ns");
   //wiredriftHisto->Draw("colz");

   //tot
   driftTotHisto->GetXaxis()->SetTitle("Driftzeit / ns");
   driftTotHisto->GetYaxis()->SetTitle("TOT / ns");
   //driftTotHisto->Draw("colz");

   //odb
   odb->GetXaxis()->SetTitle("Driftzeit / ns");
   odb->GetYaxis()->SetTitle("Abstand / mm");
   //odb->Draw(); 

   wireHisto->GetXaxis()->SetTitle("Drahtnummer");
   wireHisto->GetYaxis()->SetTitle("Trefferanzahl");
   //wireHisto->Draw(); 

    alphaHisto->GetXaxis()->SetTitle("Winkel / rad");
    alphaHisto->GetYaxis()->SetTitle("Trefferanzahl");
    alphaHisto->Draw();

}



int main(int argc, char** argv) {
  TROOT root("app","app");
  Int_t dargc=1;
  char** dargv = &argv[0];
  TRint *app = new TRint("app", &dargc, dargv);
  //TRint *app = new TRint("app", 0, NULL);
  TCanvas *c1 = new TCanvas("c", "c", 800, 600);
  TFile *f=new TFile(argv[1]);
  TTree *tree=(TTree*)f->FindObjectAny("t");
  //tree->Dump();
  analysis* ana = new analysis(tree);
  ana->Loop();
 
  app->Run(kTRUE);
}
