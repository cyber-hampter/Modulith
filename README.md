# ⚙️ **Modulith** 
‎
‎
‎
```
███╗   ███╗ ██████╗ ██████╗ ██╗   ██╗██╗     ██╗████████╗██╗  ██╗
████╗ ████║██╔═══██╗██╔══██╗██║   ██║██║     ██║╚══██╔══╝██║  ██║
██╔████╔██║██║   ██║██║  ██║██║   ██║██║     ██║   ██║   ███████║
██║╚██╔╝██║██║   ██║██║  ██║██║   ██║██║     ██║   ██║   ██╔══██║
██║ ╚═╝ ██║╚██████╔╝██████╔╝╚██████╔╝███████╗██║   ██║   ██║  ██║
╚═╝     ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚══════╝╚═╝   ╚═╝   ╚═╝  ╚═╝
```
‎
‎
> [!NOTE]
> Modulith is built around a clean `Feature` base class. Every feature inherits from it and is ready to implement immediately.

‎
---

## About this Project


### Overview

Modulith removes the boring and wasted time used when setting up a new C++ project in Visual Studio. To do this it creates a project name, customizable namespace, and individual feature names, as well as a global Feature Managing system. This allows you to keep a consistent and clean architecture for every project.


### Architecture

All generated projects follow the same structure:

```
YourProject/
├── main.cpp
├── Core/
│   ├── FeatureManager.hpp
│   └── FeatureManager.cpp
└── Features/
    ├── Feature1/
    │   ├── Feature1.hpp
    │   └── Feature1.cpp
    └── Feature2/
        ├── Feature2.hpp
        └── Feature2.cpp
```

### Feature Base Class

Every feature implements the `Feature` class defined in `Core/FeatureManager.hpp` and communicates through a shared lifecycle:

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

`SetEnabled` drives the lifecycle automatically calling `OnExecute` on enable and `OnDisabled` on disable. Each feature is fully self-contained in its own folder under `Features/`.

### Generated Feature Stub

Example:

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


Compile by using the built in visual studio compiler

**or** 

Compile with any C++17-capable MSVC toolchain

```
cl /std:c++17 /EHsc ProjectGenerator.cpp /Fe:modulith.exe
```


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


