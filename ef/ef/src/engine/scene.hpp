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

#pragma once

struct PointerCoords;

/* Represents a scene. A scene is an object that knows how to render itself to the
 * screen and knows how to react to input. At any moment in the game, exactly one
 * scene is active, and that scene is the one who decides what gets drawn to the
 * screen and how input is handled. See also: SceneManager */
class Scene {
public:
    // Called when graphics context is initialized. This is when textures,
    // geometry, etc should be initialized.
    virtual void OnStartGraphics();

    // Called when the graphics context is about to be shut down. Tear down
    // all geometry, textures, etc.
    virtual void OnKillGraphics();

    // Called when this scene has just been installed as the active scene.
    virtual void OnInstall();

    // Called when it's time to draw a frame to the screen.
    virtual void DoFrame();

    // Called when this scene is about to be uninstalled as the active scene.
    virtual void OnUninstall();

    // Called when screen gets resized
    virtual void OnScreenResized(int width, int height);

    // Called when game is paused (e.g. onPause())
    virtual void OnPause();

    // Called when game is resumed (e.g. onResumed())
    virtual void OnResume();

    // Destructor
    virtual ~Scene();
};
