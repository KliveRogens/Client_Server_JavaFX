#include "GCS_ControlValve.h"

GCS_ControlValve::GCS_ControlValve()
{
	// Расчетный тип блока
    BlockCalcType = E_NEEDINPUT;

    // Параметры блока
	createParameter("valveTimeOpen", "1");
	createParameter("valveTransferCoef", "1");
	createParameter("valveInitPos", "0");

	// Сигналы блока
    valvePositionBar = createSignal("valvePositionBar", Signal::ST_DOUBLE);

	// Порты блока
    inGasValve=createInputPort(0, "UNKNOWN_NAME", "INFO");
    outGasValve=createOutputPort(1, "UNKNOWN_NAME", "INFO");
    inGivenPosition=createInputPort(2, "UNKNOWN_NAME", "INFO");
    outPosition=createOutputPort(3, "UNKNOWN_NAME", "INFO");

	// Отказы блока
	createSituation("calcDist");

}

double GCS_ControlValve::signumFunc(double argVal)
{
    if (argVal > 0.0) return 1.0;
    if (argVal < 0.0) return -1.0;
    return 0;
}


void GCS_ControlValve::setDataNames()
{
    //выходные параметры газа
    std::vector<std::string> outGasValveName;
    outGasValveName.push_back("Объемный расход, м^3/с");
    outGasValveName.push_back("Давление, Па");
    outGasValveName.push_back("Температура, °C");
    outGasValveName.push_back("Объемная активность газа, Бк/м^3");
    outGasValveName.push_back("Объемная доля частиц в газе, отн. ед.");
    outGasValve->setDataNames(outGasValveName);
    //выходные параметры клапана
    std::vector<std::string> outPositionName;
    outPositionName.push_back("Текущее положение клапана (от 0 до 1)");
    outPosition->setDataNames(outPositionName);
}

bool GCS_ControlValve::init(std::string &error, double h)
{
    // Put your initialization here
    //проверка корректности параметров
    for (int i = 0; i < (int)Parameters.size(); i++){
        if (paramToDouble(Parameters[i])<0){
            error = "Ошибка в заполнении исходных данных!";
            return false;
        }
    }
    valvePositionPrev = paramToDouble("valveInitPos");
	setDataNames();
    return true;
}

bool GCS_ControlValve::process(double t, double h, std::string &error)
{
    // Put your calculations here
    double valvePositionGiven, valvePositionCurrent, valveTimeOpen, valveTransferCoef, valvePositionDerivative, valveInitPos,
            gasInputVolumeFlowRate, gasInputPressure, gasInputTemperature, gasInputActivity, gasInputParticleFraction,signArg,
            gasVolumeFlowRateCurrent, gasOutputPressureCurrent, gasTemperatureCurrent, gasActivityCurrent, gasParticleFractionCurrent;
    valveInitPos = paramToDouble("valveInitPos"); //начальная позиция клапана, от 0 до 1
    valveTimeOpen = paramToDouble("valveTimeOpen"); //время открытия клапана, с
    valveTransferCoef = paramToDouble("valveTransferCoef"); //передаточный коэффициент клапана
    //считывание значений на входе
    gasInputVolumeFlowRate = inGasValve->getInput()[0]; //объемный расход газа, м^3/с
    gasInputPressure = inGasValve->getInput()[1];//давление газа, Па
    gasInputTemperature = inGasValve->getInput()[2];//температура газа, град. Цел.
    gasInputActivity = inGasValve->getInput()[3];//активность газа, Бк
    gasInputParticleFraction = inGasValve->getInput()[4];//объемная доля частиц в газе, отн. ед.
    valvePositionGiven = inGivenPosition->getInput()[0]; //заданная позиция клапана (от 0 до 1)
    signArg = valvePositionGiven-valvePositionPrev; //направление движения (открывается или закрывается клапан)
    //решение уравнения клапана методом Эйлера
    valvePositionDerivative = valveTransferCoef*signumFunc(signArg)/valveTimeOpen;
    valvePositionCurrent = valvePositionPrev + valvePositionDerivative*h;
    //эта часть чтобы убрать статическую ошибку от signumFunc
    if(valvePositionGiven>valveInitPos && valvePositionCurrent > valvePositionGiven){
        valvePositionCurrent=valvePositionGiven;
    }else if(valvePositionGiven<valveInitPos && valvePositionCurrent < valvePositionGiven){
        valvePositionCurrent=valvePositionGiven;
    }
    //решение усовершенствованным методом Эйлера
    /* //тут в принципе особой разницы нет - что обычный Эйлер, что этот не очень работают из-за signumFunc. Оставлю на всякий случай
    valvePosPrime=valvePrevPos+h*(valveTransferCoef*signumFunc(valveGivenPos-valvePrevPos)/valveTimeOpen);
    valveCurrentPos = valvePrevPos + (h/2)*((valveTransferCoef*signumFunc(valveGivenPos-valvePrevPos)/valveTimeOpen)+(valveTransferCoef*signumFunc(valveGivenPos-valvePosPrime)/valveTimeOpen));
    */
    gasVolumeFlowRateCurrent = gasInputVolumeFlowRate * valvePositionCurrent;
    gasOutputPressureCurrent = gasInputPressure * valvePositionCurrent;//текущее давление на выходе фильтра, Па
    if(gasOutputPressureCurrent<gasInputPressure/2){//учет того, что давление не может так просто упасть до нуля
        gasOutputPressureCurrent = gasInputPressure/2;
    }
    gasTemperatureCurrent = gasInputTemperature;//текущая температура газа на выходе
    gasActivityCurrent = gasInputActivity;//текущая активность газа на выходе
    gasParticleFractionCurrent = gasInputParticleFraction;//текущая объемная доля частиц на выходе
    //передача значений на выходные порты
    outPosition->setOut(0,valvePositionCurrent);
    valvePositionPrev = valvePositionCurrent;


    outGasValve->setOut(0, gasVolumeFlowRateCurrent);
    outGasValve->setOut(1, gasOutputPressureCurrent);
    outGasValve->setOut(2, gasTemperatureCurrent);
    outGasValve->setOut(3, gasActivityCurrent);
    outGasValve->setOut(4, gasParticleFractionCurrent);

    valvePositionBar->Value.doubleVal = valvePositionCurrent*100;
    return true;
}


ICalcElement *Create()
{
    return new GCS_ControlValve();
}

void Release(ICalcElement *block)
{
    if(block->type() == "GCS_ControlValve") delete block;
}

std::string Type()
{
    return "GCS_ControlValve";
}