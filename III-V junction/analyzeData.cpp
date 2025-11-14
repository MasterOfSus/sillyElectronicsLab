#include <TGraph.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TF1.h>
#include <TFile.h>
#include <TCanvas.h>
#include <cmath>

void analyzeData() {
	
	std::cout << "Calibration underway." << std::endl;

	TGraphErrors* calibrationG = new TGraphErrors("data/calibrationData.txt", "%lg %lg %lg");
	calibrationG->SetName("calibrationG");
	calibrationG->SetTitle("Calibration data");
	calibrationG->GetXaxis()->SetTitle("Multimeter #DeltaV (mV)");
	calibrationG->GetYaxis()->SetTitle("Oscilloscope #DeltaV (mV)");
	calibrationG->SetMarkerStyle(29);
	calibrationG->SetMarkerColor(kGreen + 3);

	TF1* calibrationLine = new TF1("calibrationLine", "pol1", 0., 1000.);
	calibrationLine->SetParName(0, "#DeltaV offset");
	calibrationLine->SetParName(1, "Slope");
	calibrationLine->SetLineWidth(2);
	calibrationLine->SetLineColor(kViolet - 7);

	calibrationG->Fit(calibrationLine);
	TCanvas* calibrationCnvs = new TCanvas("calibrationCnvs", "Calibration Line", 800, 600);
	calibrationG->Draw();
	calibrationLine->Draw("SAME");

	TGraphErrors* IVGraphGe = new TGraphErrors("data/GeI-VData.txt", "%lg %lg %lg %lg");
	TGraphErrors* IVGraphSi = new TGraphErrors("data/SiI-VData.txt", "%lg %lg %lg %lg");
	IVGraphGe->SetName("IVGraphGe");
	IVGraphSi->SetName("IVGraphSi");
	IVGraphGe->SetTitle("Germanium junction I-V graph");
	IVGraphSi->SetTitle("Silicon junction I-V graph");
	IVGraphGe->GetXaxis()->SetTitle("#DeltaV (mV)");
	IVGraphSi->GetXaxis()->SetTitle("#DeltaV (mV)");
	IVGraphGe->GetYaxis()->SetTitle("I (mA)");
	IVGraphSi->GetYaxis()->SetTitle("I (mA)");
	IVGraphGe->SetMarkerStyle(43);
	IVGraphSi->SetMarkerStyle(33);
	IVGraphGe->SetMarkerColor(kGreen - 1);
	IVGraphSi->SetMarkerColor(kBlue - 6);
	IVGraphGe->SetDrawOption("APE");
	IVGraphSi->SetDrawOption("APE");

	TFile* output = new TFile("analyzedData.root", "RECREATE", "Analyzed data");
	IVGraphGe->Write("Non-linearized IVGraphGe");
	IVGraphSi->Write("Non-linearized IVGraphSi");

	TF1* IVGeFunct = new TF1("IVGeFunct", "pol1");
	TF1* IVSiFunct = new TF1("IVSiFunct", "pol1");
	IVGeFunct->SetParName(0, "ln(I_0)");
	IVSiFunct->SetParName(0, "ln(I_0)");
	IVGeFunct->SetParName(1, "1/#etaV_T");
	IVSiFunct->SetParName(1, "1/#etaV_T");
	IVGeFunct->SetLineColor(kOrange - 7);
	IVSiFunct->SetLineColor(kMagenta - 2);
	IVGeFunct->SetLineWidth(2);
	IVGeFunct->SetLineWidth(2);
	IVGeFunct->SetNpx(1000);
	IVSiFunct->SetNpx(1000);

	IVGeFunct->SetParameters(1., 0.02);
	IVSiFunct->SetParameters(1., 0.02);
	
	for (int i {0}; i < IVGraphGe->GetN(); ++i) {
		double y {IVGraphGe->GetPointY(i)};
		IVGraphGe->SetPointY(i, log(IVGraphGe->GetPointY(i)));
		double ey {IVGraphGe->GetErrorY(i)};
		IVGraphGe->SetPointError(i, IVGraphGe->GetErrorX(i), IVGraphGe->GetErrorY(i)/y);
		std::cout << "Error set to " << IVGraphGe->GetErrorY(i) << " for point Y " << y << " with Y error " << ey << std::endl;
	}
	for (int i {0}; i < IVGraphSi->GetN(); ++i) {
		double y {IVGraphSi->GetPointY(i)};
		IVGraphSi->SetPointY(i, log(IVGraphSi->GetPointY(i)));
		double ey {IVGraphSi->GetErrorY(i)};
		IVGraphSi->SetPointError(i, IVGraphSi->GetErrorX(i), IVGraphSi->GetErrorY(i)/y);
		std::cout << "Error set to " << IVGraphSi->GetErrorY(i) << " for point Y " << y << " with Y error " << ey << std::endl;
	}
	//	uncomment these and insert appropriate values to reduce the function fit range to the valid data
	IVGeFunct->SetRange(150., 400.);
	IVSiFunct->SetRange(350., 750.);
	IVGraphGe->Fit(IVGeFunct, "R");
	IVGraphSi->Fit(IVSiFunct, "R");

	TCanvas* IVCnvs = new TCanvas("IVCnvs", "Fit Results", 1200, 500);
	IVCnvs->Divide(2);
	IVCnvs->cd(1);
	IVGraphGe->Draw("APE");
	IVGeFunct->Draw("SAME");
	IVCnvs->cd(2);
	IVGraphSi->Draw("APE");
	IVSiFunct->Draw("SAME");


	calibrationG->Write();
	calibrationLine->Write();
	IVGraphGe->Write();
	IVGraphSi->Write();
	IVGeFunct->Write();
	IVSiFunct->Write();
	IVCnvs->Write();
	calibrationCnvs->Write();

	output->Close();

}
