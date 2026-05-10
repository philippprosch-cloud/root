{
	UInt_t messungen = 4;
	TString histogrammName = "Driftzeiten";
	TString dateien[4] = {  "20THR2853U.root", "28THR2853U.root", "1ATHR2853U.root", "1DTHR2853U.root"};
	TString titel[4] = {  "20THR2853U", "28THR2853U", "1ATHR2853U", "1DTHR2853U"};

	TLegend* leg = new TLegend(0.6, 0.5, 0.9, 0.7);
	leg->SetHeader("Spannungen");

	Bool_t first=true;
	UInt_t num = messungen;
	do {
		--num;
		TFile::Open(dateien[num]);
		TH1* plot = static_cast<TH1*>(gDirectory->FindObjectAny(histogrammName));
		plot->SetLineColor(num+1);
		if (first) {
			plot->Draw();
			first=false;
		} else {
			plot->Draw("same");
		}
		leg->AddEntry(plot, titel[num], "lep");
	} while (num != 0);

	leg->Draw("SAME");
}
