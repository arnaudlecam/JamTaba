#include "MainControllerVST.h"
#include "../midi/MidiDriver.h"
#include "../NinjamController.h"
#include "../VstPlugin/Plugin.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class AudioDriverVST : public Audio::NullAudioDriver{
public:
    AudioDriverVST()
        :NullAudioDriver(){
        globalInputRange = Audio::ChannelRange(0, 4);//4 inputs
        globalOutputRange = Audio::ChannelRange(0,2);//2 outputs

    }
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamControllerVST::NinjamControllerVST(Controller::MainController* c)
    :NinjamController(c), waitingForHostSync(false){

}

void NinjamControllerVST::waitForHostSync(){
    waitingForHostSync = true;
}

void NinjamControllerVST::syncWithHost(){
    waitingForHostSync = false;
}

void NinjamControllerVST::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){
    if(waitingForHostSync){//skip the ninjam processing if is waiting for sync
        return;
    }
    NinjamController::process(in, out, sampleRate);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainControllerVST::MainControllerVST(Persistence::Settings settings, JamtabaPlugin *plugin)
    :MainController(settings), plugin(plugin){

    qDebug() << "Creating MainControllerVST instance!";
}

MainControllerVST::~MainControllerVST(){
    if(mainWindow){
        saveLastUserSettings(mainWindow->getInputsSettings());
    }
}

int MainControllerVST::getHostBpm() const{
    if(plugin){
        return plugin->getHostBpm();
    }
    return -1;
}

void MainControllerVST::setSampleRate(int newSampleRate){
    this->sampleRate = newSampleRate;
    MainController::setSampleRate(newSampleRate);
}

Audio::AudioDriver* MainControllerVST::createAudioDriver(const Persistence::Settings &){
    return new AudioDriverVST();
}

Controller::NinjamController* MainControllerVST::createNinjamController(MainController *c){
    return new NinjamControllerVST(c);
}

Midi::MidiDriver* MainControllerVST::createMidiDriver(){
    return new Midi::NullMidiDriver();
}

void MainControllerVST::setCSS(QString css){
    qDebug() << "setting CSS";
    qApp->setStyleSheet(css);//qApp is a global variable created in dll main.
}

