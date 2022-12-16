//==============================================================================

#include "MainApplication.h"
#include "MainWindow.h"
#include "MainComponent.h"

//==============================================================================
// MainApplication members

MainApplication& MainApplication::getApp() {
  MainApplication* const app = dynamic_cast<MainApplication*>(JUCEApplication::getInstance());
  jassert(app != nullptr);
  return *app;
}

void MainApplication::closeAllAlertAndDialogWindows() {
  // delete any open alert or dialog windows.
}

//==============================================================================
// JUCEApplication overrides

MainApplication::MainApplication() {
}

const String MainApplication::getApplicationName() {
  return ProjectInfo::projectName;
}

const String MainApplication::getApplicationVersion() {
  return ProjectInfo::versionString;
}

bool MainApplication::moreThanOneInstanceAllowed() {
  return false;
}

void MainApplication::initialise(const String& commandLine) {
   auto bugs = audioDeviceManager.initialise(0,2,nullptr, true);
  // initialize the audio device manager

  // use jassert to ensure audioError is empty
    jassert(bugs.isEmpty());
  // Create the application window.
  mainWindow = std::make_unique<MainWindow>(getApplicationName());
}

void MainApplication::shutdown() {
  // Delete our main window
  mainWindow = nullptr;
}

void MainApplication::systemRequestedQuit() {
  closeAllAlertAndDialogWindows();
  quit();
}

void MainApplication::anotherInstanceStarted(const String& commandLine) {
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MainApplication)
