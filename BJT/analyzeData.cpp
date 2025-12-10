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

	TGraphErrors* IVGraph100 = new TGraphErrors("data/I-VData100.txt", "%lg %lg %lg %lg");
	TGraphErrors* IVGraph200 = new TGraphErrors("data/I-VData200.txt", "%lg %lg %lg %lg");
	IVGraph100->SetName("IVGraph100");
	IVGraph200->SetName("IVGraph200");
	IVGraph100->SetTitle("100 #muA base I-V graph");
	IVGraph200->SetTitle("200 #muA base I-V graph");
	IVGraph100->GetXaxis()->SetTitle("I (mA)");
	IVGraph200->GetXaxis()->SetTitle("I (mA)");
	IVGraph100->GetYaxis()->SetTitle("#DeltaV (V)");
	IVGraph200->GetYaxis()->SetTitle("#DeltaV (V)");
	IVGraph100->SetMarkerStyle(43);
	IVGraph200->SetMarkerStyle(33);
	IVGraph100->SetMarkerSize(2);
	IVGraph200->SetMarkerSize(2);
	IVGraph100->SetMarkerColor(kGreen - 1);
	IVGraph200->SetMarkerColor(kBlue - 6);
	IVGraph100->SetDrawOption("APE");
	IVGraph200->SetDrawOption("APE");

	TFile* output = new TFile("analyzedData.root", "RECREATE", "Analyzed data");
	IVGraph100->Write("IVGraph100");
	IVGraph200->Write("IVGraph200");

	TF1* IV100Fct = new TF1("IV100Fct", "pol1");
	TF1* IV200Fct = new TF1("IV200Fct", "pol1");
	IV100Fct->SetParName(0, "early V_{A} tension");
	IV200Fct->SetParName(0, "early V_{A} tension");
	IV100Fct->SetParName(1, "exit resistance");
	IV200Fct->SetParName(1, "exit resistance");
	IV100Fct->SetLineColor(kOrange + 5);
	IV200Fct->SetLineColor(kGreen + 3);
	IV100Fct->SetLineWidth(2);
	IV200Fct->SetLineWidth(2);
	IV100Fct->SetNpx(1000);
	IV200Fct->SetNpx(1000);
	IV100Fct->SetRange(-4., -1.);
	IV200Fct->SetRange(-4., -1.);

	// Fitting
	IVGraph100->Fit(IV100Fct, "R");
	std::cout << "Resulting exit conductance: " << 1/IV100Fct->GetParameter(1) << " +/- " << IV100Fct->GetParError(1)/std::pow(IV100Fct->GetParameter(1), 2.) << std::endl;
	IVGraph200->Fit(IV200Fct, "R");
	std::cout << "Resulting exit conductance: " << 1/IV200Fct->GetParameter(1) << " +/- " << IV200Fct->GetParError(1)/std::pow(IV200Fct->GetParameter(1), 2.) << std::endl;
	double gainV {};
	std::cout << "Input 200 uA base current actual value: ";
	double IB200 {};
	std::cin >> IB200;
	std::cout << "Input 200 uA base current error: ";
	double IB200E {};
	std::cin >> IB200E;
	std::cout << "Input 100 uA base current actual value: ";
	double IB100 {};
	std::cin >> IB100;
	std::cout << "Input 100 uA base current error: ";
	double IB100E {};
	std::cin >> IB100E;
	char repeat {'y'};
	double gain {};
	double gainErr {};
	while (repeat) {
		std::cout << "Input Voltage for gain evaluation: ";
		std::cin >> gainV;
		gain = 
			(IV200Fct->Eval(gainV) - IV100Fct->Eval(gainV)) / (IB200 - IB100)
		;
		gainErr = {
			std::abs((IV200Fct->EvalUncertainty(gainV) + IV100Fct->EvalUncertainty(gainV)) / (IB200 - IB100)) +
			std::abs(gain/(IB200 - IB100) * (IB200E + IB100E))
		};
		std::cout << "Gain for V = " << gainV << ": " << gain << " +/- " << gainErr << std::endl;
		std::cout << "Repeat? (y/n) ";
		std::cin >> repeat;
		if (repeat != 'y') {
			break;
		}
	}

	TCanvas* IVCnvs = new TCanvas("IVCnvs", "Fit Results", 1200, 500);
	IVCnvs->Divide(2);
	IVCnvs->cd(1);
	IVGraph100->Draw("APE");
	IV100Fct->Draw("SAME");
	IVCnvs->cd(2);
	IVGraph200->Draw("APE");
	IV200Fct->Draw("SAME");

	TMultiGraph* IVGraphs = new TMultiGraph("IVGraphs", "I-V graphs for 100 and 200 #muA base currents");
	IVGraphs->Add(IVGraph100);
	IVGraphs->Add(IVGraph200);
	IVGraphs->GetYaxis()->SetTitle("I (mA)");
	IVGraphs->GetXaxis()->SetTitle("#DeltaV (V)");
	TCanvas* IVCompCnvs = new TCanvas("IVCompCnvs", "IV comparison canvas.", 800, 600);
	TLegend* IVCompLgnd = new TLegend(0.55, 0.12, 0.9, 0.33);
	IVCompLgnd->AddEntry(IVGraph100, "100 #muV I_{B} exp. data", "lep");
	IVCompLgnd->AddEntry(IV100Fct, "100 #muV I_{B} fit function (drawn in fit range)");
	IVCompLgnd->AddEntry(IVGraph200, "200 #muV I_{B} exp. data", "lep");
	IVCompLgnd->AddEntry(IV200Fct, "200 #muV I_{B} fit function (drawn in fit range)");
	IVCompCnvs->cd();
	IVGraphs->SetDrawOption("APE");
	IVGraphs->Draw("APE");
	IVCompLgnd->Draw("SAME");
	IVCompCnvs->Write();

	IVGraph100->Write();
	IVGraph200->Write();
	IV100Fct->Write();
	IV200Fct->Write();
	IVCnvs->Write();

	output->Close();
}
