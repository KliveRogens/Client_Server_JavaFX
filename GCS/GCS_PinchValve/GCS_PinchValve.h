#ifndef PROJECT_H
#define PROJECT_H

#include "../../CalcElement/CalcElement.h"
#include "../../include/IPort.h"

extern "C" ICalcElement* __declspec(dllexport) Create();
extern "C" void __declspec(dllexport) Release(ICalcElement *block);
extern "C" std::string __declspec(dllexport) Type();

class GCS_PinchValve : public CalcElement
{
public:
    GCS_PinchValve();

    // ICalcElement interface
public:
    virtual bool process(double t, double h, std::string &error);
    virtual bool init(std::string &error, double h);
	void setDataNames();
	
private:
	// Put your variables here
    IInputPort *inGasValve, *inGivenPosition;
    IOutputPort *outGasValve, *outPosition;

    double signumFunc(double argVal);
    Signal *valvePositionBar;
    double valvePositionPrev;
};

#endif // PROJECT_H
