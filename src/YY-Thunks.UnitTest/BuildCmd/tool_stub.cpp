// Native executables preserve signed Windows exit statuses, unlike batch wrappers.
#include <cstdlib>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <string>

int wmain(int argc, wchar_t** argv)
{
    std::string stage;
    const auto tool = std::filesystem::path(argv[0]).stem().wstring();
    if (tool == L"msbuild")
        stage = "msbuild";
    else if (tool == L"cl")
    {
        stage = "cl-object";
        for (int index = 1; index < argc; ++index)
            if (std::wstring(argv[index]).find(L"/FoLib") == 0)
                stage = "cl-library";
    }
    else
    {
        bool library = false;
        for (int index = 1; index < argc; ++index)
        {
            const std::wstring argument(argv[index]);
            if (argument == L"FixObj" || argument == L"AppendWeak")
                stage.assign(argument.begin(), argument.end());
            if (argument == L"BuildYY_ThunksLibraries")
                stage = "library";
            library = library || argument.find(L"Lib\\") != std::wstring::npos;
        }
        if (stage != "library")
            stage += library ? "-library" : "-object";
    }

    const auto log = std::filesystem::path(_wgetenv(L"YY_BUILD_TEST_LOG"));
    int occurrence = 1;
    {
        std::ifstream previous(log);
        for (std::string line; std::getline(previous, line);)
            if (line == stage)
                ++occurrence;
    }
    {
        std::ofstream output(log, std::ios::app);
        output << stage << '\n';
        if (!output)
            return 99;
    }
    const auto failure = std::getenv("YY_BUILD_TEST_STAGE");
    if (failure && stage == failure
        && occurrence == std::atoi(std::getenv("YY_BUILD_TEST_OCCURRENCE")))
        return std::atoi(std::getenv("YY_BUILD_TEST_CODE"));
    return 0;
}
