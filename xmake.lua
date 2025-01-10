local main_bin_name = "gd-tools"
set_license("GPL-3.0")
set_languages("c++23")
set_toolchains("gcc")

set_warnings("allextra", "error")
add_cxxflags("clang::-Wno-c++98-compat")
add_cxxflags("gcc::-Wno-error=maybe-uninitialized") -- temp build fix

add_rules("mode.debug", "mode.release")

-- clangd will look in subdirectories named build/.
-- https://clangd.llvm.org/installation#project-setup
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

add_requires("cpr >= 1.11", {configs = {ssl = true}})
add_requires("cpp-subprocess")
add_requires("nlohmann_json", "marisa", "rdricpp", "mecab")

if is_mode("debug") then
    add_defines("DEBUG")
    set_symbols("debug")
    set_optimize("none")
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
elseif is_mode("release") then
    add_defines("NDEBUG")
    set_optimize("faster") -- Arch Linux builds its packages with -O2
    add_cxflags("-fstack-protector-strong", "-fstack-clash-protection")
end

if is_host("linux") then
    -- flags that should work on clang and gcc.
    add_cxflags(
        -- "-fanalyzer",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Wconversion",
        "-Wshadow",
        "-Werror"
    )
end

local format = function(target)
    import("lib.detect.find_program")
    local clang_format = find_program("clang-format")
    if not clang_format then
        return print("Skipped clang-format run for target: %s", target:name())
    end
    local paramlist = {"--sort-includes", "-i"}
    for _, file in pairs(target:headerfiles()) do
        table.insert(paramlist, file)
    end
    for _, file in pairs(target:sourcefiles()) do
        table.insert(paramlist, file)
    end
    os.execv(clang_format, paramlist)
    print("Finished clang-format for target: %s", target:name())
end

-- Main target
target(main_bin_name)
    set_kind("binary")
    add_packages("cpr","nlohmann_json", "marisa", "rdricpp", "mecab", "cpp-subprocess")
    add_files("src/*.cpp")
    add_cxflags("-D_GLIBCXX_ASSERTIONS")
    set_pcxxheader("src/precompiled.h")

    -- Run clang-format before build
    before_build(format)

    -- Default global install dir.
    set_installdir("/usr/")

    after_install(function(target)
        local function maybe_rm(file)
            if os.isfile(file) or os.islink(file) then os.rm(file) end
        end

        local bin_dir = path.join(target:installdir(), "/bin/")
        local variants = { "gd-ankisearch", "gd-echo", "gd-massif", "gd-images", "gd-marisa", "gd-mecab", }

        -- Link alternative names
        -- to enable calling `gd-ankisearch` instead of more verbose `gd-tools ankisearch`, etc.
        local link
        for _, link in pairs(variants) do
            link = path.join(bin_dir, link)
            maybe_rm(link)
            os.ln(main_bin_name, link)
        end
        print("Created symlinks.")

        -- Copy fonts
        local fonts_dir = path.join(target:installdir(), "/share/fonts/", main_bin_name)
        if not os.isdir(fonts_dir) then os.mkdir(fonts_dir) end
        os.cp("res/*.ttf", fonts_dir)
        print("Installed fonts.")

        -- Copy dictionary files
        local share_dir = path.join(target:installdir(), "/share/", main_bin_name)
        if not os.isdir(share_dir) then os.mkdir(share_dir) end
        os.cp("res/*.dic", share_dir)
        print("Installed dictionary files.")

        -- Copy sh files
        for _, shell_file in pairs(os.files("src/*.sh")) do
            local destination = path.join(target:installdir(), "bin", path.basename(shell_file))
            maybe_rm(destination)
            os.cp(shell_file, destination)
            os.runv("chmod", {"755", "--", destination})
        end
        print("Installed shell scripts.")
    end)

    before_run(function (target)
        print("Running %s", target:targetfile())
    end)
target_end()

-- Tests
-- xmake f --tests=y
option("tests", {default = false, description = "Enable tests"})

if has_config("tests") then
    -- system = false is required to pull the package from xrepo.
    -- without the main component build will fail with "undefined reference to 'main'."
    --add_requires("catch2 3.x", { system = false })
    add_requires("catch2 3.x")

    -- Tests target
    target("tests")
        set_kind("binary")
        add_packages("cpr", "nlohmann_json", "marisa", "catch2", "rdricpp", "mecab", "cpp-subprocess")
        add_files("src/*.cpp", "tests/*.cpp")
        remove_files("src/main.cpp")
        set_pcxxheader("src/precompiled.h")
        add_includedirs("src")

        -- Run clang-format before build
        before_build(format)

        before_run(function (target)
            print("Running unit tests on target: %s", target:name())
        end)
    target_end()
end

-- Describe the rdricpp dependency
package("rdricpp")
    set_homepage("https://github.com/Ajatt-Tools/rdricpp")
    set_description("Rikaitan Deinflector Reference Implementation.")
    set_license("GPL-3.0")

    add_urls("https://github.com/Ajatt-Tools/rdricpp/archive/refs/tags/$(version).tar.gz")
    add_versions("v0.1", "89a2ebb2f3c21fdd5f8177a507a09d6b23cd8adf1328c1a4c5dfad6c91d35878")
    add_versions("v0.2", "5643bbfba2cc4892ead1e04d2fe06a571feb2868f28abfbaffbab07088e8aaac")
    add_versions("v0.3", "b43fec2ac123b4abfe9bf5c143f397c3623cceb5e9daebb242d6eb4249fe13d9")

    on_install(function (package)
        import("package.tools.xmake").install(package)
    end)
package_end()

package("cpp-subprocess")
    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/arun11299/cpp-subprocess")
    set_description("Subprocessing with modern C++.")
    set_license("MIT")

    set_urls("https://github.com/arun11299/cpp-subprocess.git")
    add_versions("2024.01.25", "4025693decacaceb9420efedbf4967a04cb028e7")

    add_links("cpp-subprocess")

    on_install(function (package)
        os.cp("subprocess.hpp", package:installdir("include"))
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <subprocess.hpp>
            #include <cstring>
            #include <thread>
            namespace sp = subprocess;
            int main() {
                auto obuf = sp::check_output({"ls", "-l"});
                std::cout << "Data : " << obuf.buf.data() << std::endl;
                std::cout << "Data len: " << obuf.length << std::endl;
            }
        ]]}, {configs = {languages = "c++23"}}))
    end)
package_end()
