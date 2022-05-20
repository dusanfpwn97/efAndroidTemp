#include "entry_point.h"

#include <src/application.h>

void StartApp(struct android_app *app)
{

    auto mainApp = ef::createApplication();

    mainApp->run(app);

    delete mainApp;
}

int main(int argc, char** argv)
{
    StartApp(nullptr);
}

extern "C" {
void android_main(struct android_app *app);
}

/*
    android_main (not main) is our game entry function, it is called from
    the native app glue utility code as part of the onCreate handler.
*/



void android_main(struct android_app *app){

    StartApp(app);
}



