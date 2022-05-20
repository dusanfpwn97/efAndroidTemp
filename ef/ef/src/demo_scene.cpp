/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "demo_scene.hpp"
#include "src/engine/native_engine.hpp"


#include <functional>

extern "C" {
#include <GLES2/gl2.h>
}


DemoScene::DemoScene() {

}

DemoScene::~DemoScene() {
}

void DemoScene::OnStartGraphics() {

}

void DemoScene::OnKillGraphics() {
}

void DemoScene::OnScreenResized(int width, int height) {
}

void DemoScene::DoFrame() {
    // Make sure the controller status callback is registered before calling Paddleboat_update

    Paddleboat_update(NativeEngine::GetInstance()->GetJniEnv());

    // clear screen
    glClearColor(0.1f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_DEPTH_TEST);
}

void DemoScene::OnInstall() {


}

void DemoScene::OnUninstall() {
    ALOGI("Clearing status and motion data callbacks");
    Paddleboat_setControllerStatusCallback(nullptr, nullptr);
    Paddleboat_setMotionDataCallback(nullptr, nullptr);

}
