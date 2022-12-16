//==============================================================================

#include "MainApplication.h"
#include "MainWindow.h"
#include "MainComponent.h"

MainWindow::MainWindow(String name)
: DocumentWindow(name, Colours::white, DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
 //   ResizableWindow::setContentOwned(new MainComponent(), true);
    //resizeable and can use bottom right corner for resizing
    ResizableWindow::setResizable (true, true);
    ResizableWindow::setResizeLimits ( 600,400,1200,800);
    centreWithSize (600, 400);
    setVisible(true);

#if JUCE_WINDOWS || JUCE_LINUX
   setMenuBar(MainApplication::getApp().getMenuBarModel());
#endif

   auto maincomponent = std::make_unique<MainComponent>();
   setContentOwned(maincomponent.release(), false);

 
#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
   setResizable(true, true);
   setResizeLimits(500, 250, 1000, 500);
   centreWithSize(getWidth(), getHeight());
#else // JUCE_IOS || JUCE_ANDROID
   setFullScreen (true);
#endif
   // make the window visible
   setVisible(true);
}



void MainWindow::closeButtonPressed() {
  // when the main window is closed signal the app to exit
  JUCEApplication::getInstance()->systemRequestedQuit();
}

