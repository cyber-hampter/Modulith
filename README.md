# ⚙️ Modulith

A command-line Visual Studio C++ project generator. Scaffold complete `.sln`, `.vcxproj`, and modular feature files instantly from a single prompt.

```
███╗   ███╗ ██████╗ ██████╗ ██╗   ██╗██╗     ██╗████████╗██╗  ██╗
████╗ ████║██╔═══██╗██╔══██╗██║   ██║██║     ██║╚══██╔══╝██║  ██║
██╔████╔██║██║   ██║██║  ██║██║   ██║██║     ██║   ██║   ███████║
██║╚██╔╝██║██║   ██║██║  ██║██║   ██║██║     ██║   ██║   ██╔══██║
██║ ╚═╝ ██║╚██████╔╝██████╔╝╚██████╔╝███████╗██║   ██║   ██║  ██║
╚═╝     ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚══════╝╚═╝   ╚═╝   ╚═╝  ╚═╝
```

> [!IMPORTANT]
> Modulith targets **Visual Studio 2019** (toolset v142) and **x64** builds only. Generated projects are Windows-specific.

> [!NOTE]
> Modulith is built around a clean abstract `Feature` base class. Every generated feature inherits from it and is ready to implement immediately.

---

## About this Project

### Overview

Modulith removes the repetitive boilerplate of setting up a new modular C++ project in Visual Studio. Give it a project name, a namespace, and your feature names — it generates a complete, loadable solution in seconds with a consistent architecture across every project.

### Architecture

All generated projects follow the same structure:

```
YourProject/
├── main.cpp
├── YourProject.sln
├── YourProject.vcxproj
├── YourProject.vcxproj.filters
├── Core/
│   ├── FeatureManager.hpp
│   └── FeatureManager.cpp
└── Features/
    ├── FeatureA/
    │   ├── FeatureA.hpp
    │   └── FeatureA.cpp
    └── FeatureB/
        ├── FeatureB.hpp
        └── FeatureB.cpp
```

### Feature Base Class

Every feature implements the `Feature` abstract class defined in `Core/FeatureManager.hpp` and communicates through a shared lifecycle:

```cpp
namespace YourNamespace
{
    class Feature
    {
    public:
        Feature(const std::string& name);
        virtual ~Feature() = default;
        virtual void OnExecute()  = 0;  // called when enabled
        virtual void Tick()       = 0;  // called each frame
        virtual void OnDisabled() = 0;  // called when disabled

        void SetEnabled(bool state);
        bool IsEnabled() const;
    protected:
        std::string m_Name;
        bool        m_Enabled = false;
    };
}
```

`SetEnabled` drives the lifecycle automatically — calling `OnExecute` on enable and `OnDisabled` on disable. Each feature is fully self-contained in its own folder under `Features/`.

### Generated Feature Stub

Each feature is scaffolded as a pair of files ready to implement:

```cpp
// ESP.hpp
#pragma once
#include "../../Core/FeatureManager.hpp"

namespace MH
{
    class ESP : public Feature
    {
    public:
        ESP();
        void OnExecute()  override;
        void Tick()       override;
        void OnDisabled() override;
    };
}
```

## Usage

### Build Modulith

Compile with any C++17-capable MSVC toolchain

```
cl /std:c++17 /EHsc ProjectGenerator.cpp /Fe:modulith.exe
```

**or** 

Compile by using the built in visual studio compiler


### Create a Project

```
1) Create Project
2) Add New Feature
Select: 1

Base Path:     C:\Projects
Project Name:  MyProject
Namespace:     MP
Feature Count: 2
Feature 1 Name: ESP
Feature 2 Name: AimAssist

Visual Studio project generated successfully at: C:\Projects\MyProject
```

### Add a Feature to an Existing Project

```
Select: 2

Project Path:     C:\Projects
Project Name:     MyProject
Namespace:        MP
New Feature Name: Radar

Feature "Radar" created successfully
Note: Add the new files to your .vcxproj manually, or re-run option 1 to regenerate the project.
```

---

## About

⚙️ **Modulith** is a C++ project creator designed to simplify cheat development
