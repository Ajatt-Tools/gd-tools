local main_bin_name = "gd-tools"
set_languages("c++23")
set_toolchains("gcc")

set_warnings("allextra", "error")
add_cxxflags("clang::-Wno-c++98-compat")

add_rules("mode.debug", "mode.release")
add_rules("mode.ubsan", "mode.check")

add_requires("cpr >= 1.10.5", "fmt >= 10", "nlohmann_json", "marisa")

if is_mode("debug") then
    add_defines("DEBUG")
    set_symbols("debug")
    set_optimize("none")
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
    add_packages("cpr", "fmt", "nlohmann_json", "marisa")
    add_files("src/*.cpp")
    add_cxflags("-D_GLIBCXX_ASSERTIONS")
    set_pcxxheader("src/precompiled.h")
    add_headerfiles("src/*.h")

    -- Run clang-format before build
    before_build(format)

    -- Default global install dir.
    set_installdir("/usr/")

    after_install(function(target)
        local function maybe_rm(file)
            if os.isfile(file) or os.islink(file) then os.rm(file) end
        end

        local bin_dir = path.join(target:installdir(), "/bin/")
        local variants = {
            "gd-ankisearch", "gd-echo", "gd-massif", "gd-images", "gd-marisa"
            -- "gd-mecab",
        }

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
        end
        print("Installed shell scripts.")
    end)

    before_run(function (target)
        print("Running %s", target:targetfile())
    end)
target_end()

option("tests", {default = false, description = "Enable tests"})
if has_config("tests") then
    add_requires("catch2")
end

-- Tests target
target("tests")
    set_kind("binary")
    add_packages("cpr", "fmt", "nlohmann_json", "marisa", "catch2")
    add_files("src/*.cpp", "tests/*.cpp")
    remove_files("src/main.cpp")
    set_pcxxheader("src/precompiled.h")
    add_headerfiles("src/*.h")
    add_includedirs("src")

    -- Run clang-format before build
    before_build(format)

    before_run(function (target)
        print("Running unit tests on target: %s", target:name())
    end)
target_end()
