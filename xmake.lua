local main_bin_name = "gd-tools"
local bin_variants = { "gd-ankisearch", "gd-echo", "gd-massif", "gd-images", "gd-marisa", "gd-mecab", }
local package_version = os.getenv("GD_TOOLS_VERSION") or "0.0.0"
local font_files = "res/*.ttf"
local dictionary_files = "res/*.dic"
local shell_files = "src/*.sh"
local fonts_prefix = path.join("share/fonts", main_bin_name)
local data_prefix = path.join("share", main_bin_name)

-- Return whether the host is Ubuntu, which provides the GCC 14 toolchain and system curl packages.
local function is_ubuntu()
    return is_host("linux") and linuxos.name() == "ubuntu"
end

-- Respect explicit curl linkage requests; otherwise prefer Ubuntu's maintained shared library.
local function use_system_curl()
    local requested = os.getenv("GD_TOOLS_USE_SYSTEM_CURL")
    return requested == "y" or (requested ~= "n" and is_ubuntu())
end

set_xmakever("2.9.3")
set_license("GPL-3.0")
set_languages("c++23")
if is_ubuntu() and not os.getenv("GD_TOOLS_TOOLCHAIN") then
    set_toolchains("gcc-14")
else
    set_toolchains(os.getenv("GD_TOOLS_TOOLCHAIN") or "gcc")
end

set_warnings("allextra", "error")
add_cxxflags("clang::-Wno-c++98-compat")
add_cxxflags("gcc::-Wno-error=maybe-uninitialized") -- temp build fix

add_rules("mode.debug", "mode.release")

-- xmake f --tests=y
option("tests", {default = false, description = "Enable tests"})

includes("@builtin/xpack")

-- clangd will look in subdirectories named build/.
-- https://clangd.llvm.org/installation#project-setup
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

add_requires("cpr >= 1.11", {configs = {ssl = true}})
if use_system_curl() then
    add_requireconfs("cpr.libcurl", {system = true, override = true, version = ">=7.64.0"})
end
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

-- Remove an installed file or dangling alias without failing when it is absent.
local function remove_if_present(xos, file)
    if xos.isfile(file) or xos.islink(file) then xos.rm(file) end
end

-- Install short command aliases for the multicall binary.
local function install_variants(xos, target)
    local bin_dir = path.join(target:installdir(), "bin")
    for _, variant in ipairs(bin_variants) do
        local link = path.join(bin_dir, variant)
        remove_if_present(xos, link)
        xos.ln(main_bin_name, link)
    end
end

-- Copy one resource pattern into its installation directory.
local function install_resources(xos, files, directory)
    if not xos.isdir(directory) then xos.mkdir(directory) end
    xos.cp(files, directory)
end

-- Install shell commands without their source .sh extensions.
local function install_shell_files(xos, target)
    local bin_dir = path.join(target:installdir(), "bin")
    for _, shell_file in ipairs(xos.files(shell_files)) do
        local destination = path.join(bin_dir, path.basename(shell_file))
        remove_if_present(xos, destination)
        xos.cp(shell_file, destination)
        xos.runv("chmod", {"755", "--", destination})
    end
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
        -- Link alternative names to enable calling `gd-ankisearch`
        -- instead of more verbose `gd-tools ankisearch`, etc.
        install_variants(os, target)
        print("Created symlinks.")

        -- Copy fonts
        install_resources(os, font_files, path.join(target:installdir(), fonts_prefix))
        print("Installed fonts.")

        -- Copy dictionary files
        install_resources(os, dictionary_files, path.join(target:installdir(), data_prefix))
        print("Installed dictionary files.")

        -- Copy sh files
        install_shell_files(os, target)
        print("Installed shell scripts.")
    end)

    -- Xmake does not track files created by after_install.
    after_uninstall(function(target)
        -- Remove every file created outside Xmake's tracked target installation.
        local bin_dir = path.join(target:installdir(), "bin")
        for _, variant in ipairs(bin_variants) do
            remove_if_present(os, path.join(bin_dir, variant))
        end
        for _, shell_file in ipairs(os.files(shell_files)) do
            remove_if_present(os, path.join(bin_dir, path.basename(shell_file)))
        end
        os.rm(path.join(target:installdir(), fonts_prefix))
        os.rm(path.join(target:installdir(), data_prefix))
        print("Removed gd-tools installation files.")
    end)

    before_run(function (target)
        print("Running %s", target:targetfile())
    end)
target_end()

-- XPack cannot translate the imperative after_install hook, so its payload is declared explicitly.
xpack(main_bin_name)
    set_formats("deb")
    set_inputkind("binary")
    set_specfile("packaging/debian")
    set_version(package_version)
    set_basename(main_bin_name)
    set_title("GoldenDict tools")
    set_description("A set of helpful programs to enhance GoldenDict for immersion learning.")
    set_author("Ajatt-Tools and contributors")
    set_maintainer("Ren Tatsumoto <tatsu@autistici.org>")
    set_homepage("https://github.com/Ajatt-Tools/gd-tools")
    set_license("GPL-3.0")
    set_licensefile("LICENSE")
    add_targets(main_bin_name)
    add_installfiles(font_files, {prefixdir = fonts_prefix})
    add_installfiles(dictionary_files, {prefixdir = data_prefix})
    for _, shell_file in ipairs(os.files(shell_files)) do
        add_installfiles(shell_file, {prefixdir = "bin", filename = path.basename(shell_file)})
    end
    add_installfiles("LICENSE", {prefixdir = "share/licenses/gd-tools"})
    add_installfiles("README.md", {prefixdir = "share/doc/gd-tools", filename = "README"})
    add_installfiles("res/ArmedLemon-LICENSE.txt", {prefixdir = "share/doc/gd-tools"})

    after_installcmd(function(package, batchcmds)
        for _, variant in ipairs(bin_variants) do
            batchcmds:runv("ln", {"-s", main_bin_name, path.new(package:installdir(path.join("bin", variant)))})
        end
    end)
xpack_end()

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
