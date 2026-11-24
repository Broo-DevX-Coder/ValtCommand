#include "test_helpers.hpp"
#include <filesystem>

int main(int argc, char** argv) {
    // Resolve bundled documentation relative to the test executable, regardless
    // of the caller's current working directory.
    const auto docs = std::filesystem::path("docs") / "V1.0.0" /
                      "Language-references" / "examples";
    if (!std::filesystem::exists(docs) && argc > 0) {
        const auto executable_dir = std::filesystem::absolute(argv[0]).parent_path();
        if (std::filesystem::exists(executable_dir / docs)) {
            std::filesystem::current_path(executable_dir);
        }
    }

    // Run every suite even when another suite records a failure.
    tests::test_results();
    tests::test_functions();
    tests::test_errors();
    tests::test_documented_examples();
    tests::test_runtime_api();

    int failures = 0;
    for (const auto& suite : tests::suites()) {
        std::cout << suite.name << ": " << suite.passed << " passed, "
                  << suite.failed << " failed, " << suite.skipped << " skipped\n";
        failures += suite.failed;
    }
    return failures == 0 ? 0 : 1;
}
