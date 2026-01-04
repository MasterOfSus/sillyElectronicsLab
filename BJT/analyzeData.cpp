#include <cmath>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMath.h>
#include <TAxis.h>
#include <TMultiGraph.h>
#include <TFile.h>
#include <TF1.h>

void analyzeData() {

	TGraphErrors* IVGraph50 = new TGraphErrors("data/I-VData50.txt", "%lg %lg %lg %lg");
	TGraphErrors* IVGraph100 = new TGraphErrors("data/I-VData100.txt", "%lg %lg %lg %lg");
	IVGraph50->SetName("IVGraph50");
	IVGraph100->SetName("IVGraph100");
	IVGraph50->SetTitle("100 #muA base I-V graph");
	IVGraph100->SetTitle("200 #muA base I-V graph");
	IVGraph50->GetXaxis()->SetTitle("-I (mA)");
	IVGraph100->GetXaxis()->SetTitle("-I (mA)");
	IVGraph50->GetYaxis()->SetTitle("-#DeltaV (V)");
	IVGraph100->GetYaxis()->SetTitle("-#DeltaV (V)");
	IVGraph50->SetMarkerStyle(43);
	IVGraph100->SetMarkerStyle(33);
	IVGraph50->SetMarkerSize(2);
	IVGraph100->SetMarkerSize(2);
	IVGraph50->SetMarkerColor(kGreen - 1);
	IVGraph100->SetMarkerColor(kBlue - 6);
	IVGraph50->SetDrawOption("APE");
	IVGraph100->SetDrawOption("APE");

	TFile* output = new TFile("analyzedData.root", "RECREATE", "Analyzed data");
	IVGraph50->Write("IVGraph50");
	IVGraph100->Write("IVGraph50");

	TF1* IV50Fct = new TF1("IV50Fct", "pol1");
	TF1* IV100Fct = new TF1("IV100Fct", "pol1");
	IV50Fct->SetParName(0, "early V_{A} tension (V)");
	IV100Fct->SetParName(0, "early V_{A} tension (V)");
	IV50Fct->SetParName(1, "exit resistance (kOhm)");
	IV100Fct->SetParName(1, "exit resistance (kOhm)");
	IV50Fct->SetLineColor(kOrange + 5);
	IV100Fct->SetLineColor(kGreen + 3);
	IV50Fct->SetLineWidth(2);
	IV100Fct->SetLineWidth(2);
	IV50Fct->SetNpx(1000);
	IV100Fct->SetNpx(1000);
	IV50Fct->SetRange(13.5, 15.5);
	IV100Fct->SetRange(22., 26.);

	// Fitting
	std::cout << "50 muA fit results:" << std::endl;
	IVGraph50->Fit(IV50Fct, "R");
	std::cout << "Resulting exit conductance: (" << 1/IV50Fct->GetParameter(1) << " +/- " << IV50Fct->GetParError(1)/std::pow(IV50Fct->GetParameter(1), 2.) << ") mS" << std::endl;
	std::cout << "100 muA fit results:" << std::endl;
	IVGraph100->Fit(IV100Fct, "R");
	std::cout << "Resulting exit conductance: (" << 1/IV100Fct->GetParameter(1) << " +/- " << IV100Fct->GetParError(1)/std::pow(IV100Fct->GetParameter(1), 2.) << ") mS" << std::endl;
	size_t gainI {8};
	// in mA
	double IB100 {1E-1};
	double IB100E {2.15E-2};
	double IB50 {5E-2};
	double IB50E {2.075E-2};
	double gain { 
		(IVGraph100->GetPointX(gainI) - IVGraph50->GetPointX(gainI)) / (IB100 - IB50)
	};
	double gainErr {
		std::abs((IVGraph100->GetErrorX(gainI) + IVGraph50->GetErrorX(gainI)) / (IB100 - IB50)) +
		std::abs(gain/(IB100 - IB50) * (IB100E + IB50E))
	};
	std::cout << "Gain for V = " << IVGraph100->GetPointY(gainI) << ": " << gain << " +/- " << gainErr << std::endl;

	TCanvas* IVCnvs = new TCanvas("IVCnvs", "Fit Results", 1200, 500);
	IVCnvs->Divide(2);
	IVCnvs->cd(1);
	IVGraph50->Draw("APE");
	IV50Fct->Draw("SAME");
	IVCnvs->cd(2);
	IVGraph100->Draw("APE");
	IV100Fct->Draw("SAME");

	TMultiGraph* IVGraphs = new TMultiGraph("IVGraphs", "Grafici I-V per correnti di base da 50 e 100 #muA");
	IVGraphs->Add(IVGraph50);
	IVGraphs->Add(IVGraph100);
	IVGraphs->GetXaxis()->SetTitle("-I (mA)");
	IVGraphs->GetYaxis()->SetTitle("-#DeltaV (V)");
	TCanvas* IVCompCnvs = new TCanvas("IVCompCnvs", "IV comparison canvas.", 800, 600);
	TLegend* IVCompLgnd = new TLegend(0.15, 0.675, 0.5, 0.85);
	IVCompLgnd->AddEntry(IVGraph50, "Dati sperimentali per I_{B} = 50 #muV", "lep");
	IVCompLgnd->AddEntry(IV50Fct, "Funzione di fit per I_{B} = 50 #muV");
	IVCompLgnd->AddEntry(IVGraph100, "Dati sperimentali per I_{B} = 100 #muV", "lep");
	IVCompLgnd->AddEntry(IV100Fct, "Funzione di fit per I_{B} = 100 #muV");
	IVCompCnvs->cd();
	IVGraphs->SetDrawOption("APE");
	IVGraphs->Draw("APE");
	IVCompLgnd->Draw("SAME");
	IVCompCnvs->Write();

	IVGraph50->Write();
	IVGraph100->Write();
	IV50Fct->Write();
	IV100Fct->Write();
	IVCnvs->Write();

	output->Close();
}
