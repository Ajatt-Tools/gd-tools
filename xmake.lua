local main_bin_name = "gd-tools"

add_rules("mode.debug", "mode.release")
add_rules("mode.ubsan", "mode.check")
add_requires("cpr >= 1.10.3", "fmt", "nlohmann_json", "marisa")

target(main_bin_name)
set_kind("binary")
set_languages("c++23")
set_warnings("allextra", "error")
set_optimize("faster")
add_packages("cpr", "fmt", "nlohmann_json", "marisa")
add_files("src/*.cpp")
add_cxflags("-D_GLIBCXX_ASSERTIONS")

set_pcxxheader("src/precompiled.h")
add_headerfiles("src/**.h")

if is_host("linux") then
    -- flags that should work on clang and gcc.
    add_cxflags("-Wall", "-Wextra", "-Wpedantic", "-Wconversion", "-Wshadow",
                "-Werror")
end
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
end
if is_mode("release") then
    add_cxflags("-fstack-protector-strong", "-fstack-clash-protection")
end

-- Run clang-format before build
before_build(function(target)
    import("lib.detect.find_program")
    local clang_format = find_program("clang-format")
    if not clang_format then
        return print("Skipped clang-format run.")
    end
    local paramlist = {"--sort-includes", "-i"}
    for _, file in pairs(target:headerfiles()) do
        table.insert(paramlist, file)
    end
    for _, file in pairs(target:sourcefiles()) do
        table.insert(paramlist, file)
    end
    os.execv(clang_format, paramlist)
    print("Finished clang-format.")
end)

-- Default global install dir.
set_installdir("/usr/")

after_install(function(target)
    local bin_dir = path.join(target:installdir(), "/bin/")
    local variants = {
        "gd-ankisearch", "gd-echo", "gd-massif", "gd-images", "gd-marisa"
        -- "gd-mecab",
    }

    -- Link alternative names
    -- to enable calling `gd-ankisearch` instead of more verbose `gd-tools ankisearch`, etc.
    local link
    for _, exe in pairs(variants) do
        link = path.join(bin_dir, exe)
        if os.isfile(link) then os.rm(link) end
        os.ln(main_bin_name, link)
    end
    print("Created symlinks.")

    -- Copy fonts
    local fonts_dir = path.join(target:installdir(), "/share/fonts/",
                                main_bin_name)
    if not os.isdir(fonts_dir) then os.mkdir(fonts_dir) end
    os.cp("res/*.ttf", fonts_dir)
    print("Installed fonts.")

    -- Copy dictionary files
    local share_dir = path.join(target:installdir(), "/share/", main_bin_name)
    if not os.isdir(share_dir) then os.mkdir(share_dir) end
    os.cp("res/*.dic", share_dir)
    print("Installed dictionary files.")

    -- Copy sh files
    for _, file in pairs(os.files("src/*.sh")) do
        os.cp(file, path.join(target:installdir(), "bin", path.basename(file)))
    end
    print("Installed shell scripts.")
end)
target_end()
