#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <windows.h>

namespace fs = std::filesystem;

// file writer
void WriteFile(const fs::path& path, const std::string& content)
{
    std::ofstream file(path);
    if (!file)
    {
        std::cerr << "Failed to write: " << path << "\n";
        return;
    }
    file << content;
}

// project guid creator (this is so dumb)
std::string GenerateGUID()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

// Core features / feature manager
void GenerateCore(const fs::path& root, const std::string& ns)
{
    fs::create_directories(root / "Core");

    std::string header =
        "#pragma once\n"
        "#include <string>\n\n"
        "namespace " + ns + "\n"
        "{\n"
        "    class Feature\n"
        "    {\n"
        "    public:\n"
        "        Feature(const std::string& name);\n"
        "        virtual ~Feature() = default;\n"
        "        virtual void OnExecute() = 0;\n"
        "        virtual void Tick() = 0;\n"
        "        virtual void OnDisabled() = 0;\n"
        "        void SetEnabled(bool state);\n"
        "        bool IsEnabled() const;\n"
        "    protected:\n"
        "        std::string m_Name;\n"
        "        bool m_Enabled = false;\n"
        "    };\n"
        "}\n";

    std::string cpp =
        "#include \"FeatureManager.hpp\"\n\n"
        "namespace " + ns + "\n"
        "{\n"
        "    Feature::Feature(const std::string& name) : m_Name(name) {}\n\n"
        "    void Feature::SetEnabled(bool state)\n"
        "    {\n"
        "        m_Enabled = state;\n"
        "        if (m_Enabled) OnExecute(); else OnDisabled();\n"
        "    }\n\n"
        "    bool Feature::IsEnabled() const { return m_Enabled; }\n"
        "}\n";

    WriteFile(root / "Core/FeatureManager.hpp", header);
    WriteFile(root / "Core/FeatureManager.cpp", cpp);
}

// each feature
void GenerateFeature(const fs::path& root, const std::string& ns, const std::string& name)
{
    fs::path dir = root / "Features" / name;
    fs::create_directories(dir);

    std::ostringstream h, cpp;

    h << "#pragma once\n"
        << "#include \"../../Core/FeatureManager.hpp\"\n\n"
        << "namespace " << ns << "\n"
        << "{\n"
        << "    class " << name << " : public Feature\n"
        << "    {\n"
        << "    public:\n"
        << "        " << name << "();\n"
        << "        void OnExecute() override;\n"
        << "        void Tick() override;\n"
        << "        void OnDisabled() override;\n"
        << "    };\n"
        << "}\n";

    cpp << "#include \"" << name << ".hpp\"\n\n"
        << "namespace " << ns << "\n"
        << "{\n"
        << "    " << name << "::" << name << "() : Feature(\"" << name << "\") {}\n\n"
        << "    void " << name << "::OnExecute() {}\n\n"
        << "    void " << name << "::Tick() {}\n\n"
        << "    void " << name << "::OnDisabled() {}\n"
        << "}\n";

    WriteFile(dir / (name + ".hpp"), h.str());
    WriteFile(dir / (name + ".cpp"), cpp.str());
}

// main file
void GenerateMain(const fs::path& root)
{
    std::string mainCPP =
        "#include <iostream>\n\n"
        "int main()\n"
        "{\n"
        "    std::cout << \"Project Initialized\" << std::endl;\n"
        "    return 0;\n"
        "}\n";
    WriteFile(root / "main.cpp", mainCPP);
}

// project generator
void GenerateVCXProj(const fs::path& root, const std::string& name, const std::vector<std::string>& features)
{
    std::string projectGUID = "{" + GenerateGUID() + "}";

    // vcxproj 
    std::ostringstream vcx;
    vcx <<
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Project DefaultTargets=\"Build\" ToolsVersion=\"16.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"

        "  <ItemGroup Label=\"ProjectConfigurations\">\n"
        "    <ProjectConfiguration Include=\"Debug|x64\">\n"
        "      <Configuration>Debug</Configuration>\n"
        "      <Platform>x64</Platform>\n"
        "    </ProjectConfiguration>\n"
        "    <ProjectConfiguration Include=\"Release|x64\">\n"
        "      <Configuration>Release</Configuration>\n"
        "      <Platform>x64</Platform>\n"
        "    </ProjectConfiguration>\n"
        "  </ItemGroup>\n"

        "  <PropertyGroup Label=\"Globals\">\n"
        "    <VCProjectVersion>16.0</VCProjectVersion>\n"
        "    <ProjectGuid>" + projectGUID + "</ProjectGuid>\n"
        "    <RootNamespace>" + name + "</RootNamespace>\n"
        "    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>\n"
        "  </PropertyGroup>\n"

        "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n"

        "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"Configuration\">\n"
        "    <ConfigurationType>Application</ConfigurationType>\n"
        "    <UseDebugLibraries>true</UseDebugLibraries>\n"
        "    <PlatformToolset>v142</PlatformToolset>\n"
        "    <CharacterSet>MultiByte</CharacterSet>\n"
        "  </PropertyGroup>\n"

        "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\" Label=\"Configuration\">\n"
        "    <ConfigurationType>Application</ConfigurationType>\n"
        "    <UseDebugLibraries>false</UseDebugLibraries>\n"
        "    <PlatformToolset>v142</PlatformToolset>\n"
        "    <WholeProgramOptimization>true</WholeProgramOptimization>\n"
        "    <CharacterSet>MultiByte</CharacterSet>\n"
        "  </PropertyGroup>\n"

        "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n"

        "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
        "    <ClCompile>\n"
        "      <WarningLevel>Level3</WarningLevel>\n"
        "      <SDLCheck>true</SDLCheck>\n"
        "      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
        "      <ConformanceMode>true</ConformanceMode>\n"
        "      <LanguageStandard>stdcpp17</LanguageStandard>\n"
        "    </ClCompile>\n"
        "    <Link>\n"
        "      <SubSystem>Console</SubSystem>\n"
        "      <GenerateDebugInformation>true</GenerateDebugInformation>\n"
        "    </Link>\n"
        "  </ItemDefinitionGroup>\n"

        "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\n"
        "    <ClCompile>\n"
        "      <WarningLevel>Level3</WarningLevel>\n"
        "      <SDLCheck>true</SDLCheck>\n"
        "      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>\n"
        "      <ConformanceMode>true</ConformanceMode>\n"
        "      <LanguageStandard>stdcpp17</LanguageStandard>\n"
        "      <FunctionLevelLinking>true</FunctionLevelLinking>\n"
        "      <IntrinsicFunctions>true</IntrinsicFunctions>\n"
        "    </ClCompile>\n"
        "    <Link>\n"
        "      <SubSystem>Console</SubSystem>\n"
        "      <EnableCOMDATFolding>true</EnableCOMDATFolding>\n"
        "      <OptimizeReferences>true</OptimizeReferences>\n"
        "    </Link>\n"
        "  </ItemDefinitionGroup>\n";

    // Core files
    vcx << "  <ItemGroup>\n";
    vcx << "    <ClCompile Include=\"main.cpp\" />\n";
    vcx << "    <ClCompile Include=\"Core\\FeatureManager.cpp\" />\n";
    for (auto& f : features)
        vcx << "    <ClCompile Include=\"Features\\" << f << "\\" << f << ".cpp\" />\n";
    vcx << "  </ItemGroup>\n";


    // Header files
    vcx << "  <ItemGroup>\n";
    vcx << "    <ClInclude Include=\"Core\\FeatureManager.hpp\" />\n";
    for (auto& f : features)
        vcx << "    <ClInclude Include=\"Features\\" << f << "\\" << f << ".hpp\" />\n";
    vcx << "  </ItemGroup>\n";

    vcx << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n"
        "</Project>\n";

    WriteFile(root / (name + ".vcxproj"), vcx.str());


    // vcxproj filter 
    std::ostringstream filters;
    filters <<
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
        "  <ItemGroup>\n"
        "    <Filter Include=\"Source Files\">\n"
        "      <UniqueIdentifier>{" << GenerateGUID() << "}</UniqueIdentifier>\n"
        "    </Filter>\n"
        "    <Filter Include=\"Header Files\">\n"
        "      <UniqueIdentifier>{" << GenerateGUID() << "}</UniqueIdentifier>\n"
        "    </Filter>\n"
        "    <Filter Include=\"Source Files\\Core\">\n"
        "      <UniqueIdentifier>{" << GenerateGUID() << "}</UniqueIdentifier>\n"
        "    </Filter>\n"
        "    <Filter Include=\"Header Files\\Core\">\n"
        "      <UniqueIdentifier>{" << GenerateGUID() << "}</UniqueIdentifier>\n"
        "    </Filter>\n";

    for (auto& f : features)
    {
        filters <<
            "    <Filter Include=\"Source Files\\Features\\" << f << "\">\n"
            "      <UniqueIdentifier>{" << GenerateGUID() << "}</UniqueIdentifier>\n"
            "    </Filter>\n"
            "    <Filter Include=\"Header Files\\Features\\" << f << "\">\n"
            "      <UniqueIdentifier>{" << GenerateGUID() << "}</UniqueIdentifier>\n"
            "    </Filter>\n";
    }

    filters << "  </ItemGroup>\n";

    filters << "  <ItemGroup>\n";
    filters << "    <ClCompile Include=\"main.cpp\"><Filter>Source Files</Filter></ClCompile>\n";
    filters << "    <ClCompile Include=\"Core\\FeatureManager.cpp\"><Filter>Source Files\\Core</Filter></ClCompile>\n";
    for (auto& f : features)
        filters << "    <ClCompile Include=\"Features\\" << f << "\\" << f << ".cpp\"><Filter>Source Files\\Features\\" << f << "</Filter></ClCompile>\n";
    filters << "  </ItemGroup>\n";

    filters << "  <ItemGroup>\n";
    filters << "    <ClInclude Include=\"Core\\FeatureManager.hpp\"><Filter>Header Files\\Core</Filter></ClInclude>\n";
    for (auto& f : features)
        filters << "    <ClInclude Include=\"Features\\" << f << "\\" << f << ".hpp\"><Filter>Header Files\\Features\\" << f << "</Filter></ClInclude>\n";
    filters << "  </ItemGroup>\n";

    filters << "</Project>\n";

    WriteFile(root / (name + ".vcxproj.filters"), filters.str());

    // sln info
    std::string solutionGUID = "{" + GenerateGUID() + "}";
    std::ostringstream sln;
    sln <<
        "Microsoft Visual Studio Solution File, Format Version 12.00\n"
        "# Visual Studio Version 16\n"
        "VisualStudioVersion = 16.0.31005.135\n"
        "MinimumVisualStudioVersion = 10.0.40219.1\n"
        "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \""
        << name << "\", \"" << name << ".vcxproj\", \"" << projectGUID << "\"\n"
        "EndProject\n"
        "Global\n"
        "  GlobalSection(SolutionConfigurationPlatforms) = preSolution\n"
        "    Debug|x64 = Debug|x64\n"
        "    Release|x64 = Release|x64\n"
        "  EndGlobalSection\n"
        "  GlobalSection(ProjectConfigurationPlatforms) = postSolution\n"
        "    " << projectGUID << ".Debug|x64.ActiveCfg = Debug|x64\n"
        "    " << projectGUID << ".Debug|x64.Build.0 = Debug|x64\n"
        "    " << projectGUID << ".Release|x64.ActiveCfg = Release|x64\n"
        "    " << projectGUID << ".Release|x64.Build.0 = Release|x64\n"
        "  EndGlobalSection\n"
        "EndGlobal\n";

    WriteFile(root / (name + ".sln"), sln.str());
}

// project creation
void CreateProject()
{
    std::string base, name, ns;
    int featureCount;

    std::cout << "Base Path: ";
    std::getline(std::cin >> std::ws, base);

    std::cout << "Project Name: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Namespace: ";
    std::getline(std::cin >> std::ws, ns);

    std::cout << "Feature Count: ";
    std::cin >> featureCount;

    fs::path root = fs::path(base) / name;
    fs::create_directories(root / "Features");

    GenerateCore(root, ns);
    GenerateMain(root);

    std::vector<std::string> features;
    for (int i = 1; i <= featureCount; i++)
    {
        std::string fname;
        std::cout << "Feature " << i << " Name: ";
        std::getline(std::cin >> std::ws, fname);
        GenerateFeature(root, ns, fname);
        features.push_back(fname);
    }

    GenerateVCXProj(root, name, features);

    std::cout << "\nVisual Studio project generated successfully at: " << root << "\n";
}

// add new feature to existing project
void AddFeature()
{
    std::string base, projectName, ns, featureName;

    std::cout << "Project Path: ";
    std::getline(std::cin >> std::ws, base);

    std::cout << "Project Name: ";
    std::getline(std::cin >> std::ws, projectName);

    std::cout << "Namespace: ";
    std::getline(std::cin >> std::ws, ns);

    std::cout << "New Feature Name: ";
    std::getline(std::cin >> std::ws, featureName);

    fs::path root = fs::path(base) / projectName;

    if (!fs::exists(root))
    {
        std::cerr << "Project path does not exist: " << root << "\n";
        return;
    }

    GenerateFeature(root, ns, featureName);

    std::cout << "\nFeature \"" << featureName << "\" created successfully ";
    std::cout << "Note: Add the new files to your .vcxproj manually, or re-run option 1 to regenerate the project.\n";
}


int main()
{
    // optional ascii (if you don't set UTF 8 the art will render incorrectly)
    SetConsoleOutputCP(CP_UTF8);

    std::cout << R"(
        ███╗   ███╗ ██████╗ ██████╗ ██╗   ██╗██╗     ██╗████████╗██╗  ██╗
        ████╗ ████║██╔═══██╗██╔══██╗██║   ██║██║     ██║╚══██╔══╝██║  ██║
        ██╔████╔██║██║   ██║██║  ██║██║   ██║██║     ██║   ██║   ███████║
        ██║╚██╔╝██║██║   ██║██║  ██║██║   ██║██║     ██║   ██║   ██╔══██║
        ██║ ╚═╝ ██║╚██████╔╝██████╔╝╚██████╔╝███████╗██║   ██║   ██║  ██║
        ╚═╝     ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚══════╝╚═╝   ╚═╝   ╚═╝  ╚═╝
    )" << "\n";

    std::cout << "1) Create Project\n";
    std::cout << "2) Add New Feature\n";
    std::cout << "Select: ";
    int choice;
    std::cin >> choice;

    if (choice == 1)
        CreateProject();

    else if (choice == 2)
        AddFeature();

    return 0;
}