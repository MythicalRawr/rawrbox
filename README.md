<p align="center">
  <img src="https://i.rawr.dev/caw_sleep.png" width=512 /><br/>
</p>

<h2>
	<p align="center">
		<a href="https://github.com/MythicalRawr/rawrbox">RawrBox</a> - A different version of <a href="https://github.com/Goofy-Penguin/Mainframe">Mainframe</a>, so brom doesn't yell at me for breaking his apps<br/>
		<a href="#LIBS">Libs</a> -
		<a href="#CMAKE">Settings</a> -
		<a href="#Dependencies">Dependencies</a> -
		<a href="#Building">Building</a>
	</p>
</h2>

# LIBS

| LIB              | NOTE                                                    |
| :--------------- | :------------------------------------------------------ |
| `RAWRBOX.RENDER` | Used for rendering (aka, contains window, stencil, etc) |
| `RAWRBOX.MATH`   | Math lib                                                |
| `RAWRBOX.UTILS`  | Utils lib                                               |

# CMAKE

| OPTION NAME                | NOTE                               |
| :------------------------- | :--------------------------------- |
| `RAWRBOX_USE_WAYLAND`      | Enables WAYLAND compiling on LINUX |
| `RAWRBOX_BUILD_SAMPLE`     | Builds the project sample          |
| `RAWRBOX_BUILD_UTIL_QHULL` | Enables QHull util                 |

# Dependencies

| LIB       | REQUIRED | NOTE                                              |
| :-------- | :------: | :------------------------------------------------ |
| bgfx      |    ✔️    |                                                   |
| bx        |    ✔️    | Required by `bgfx`                                |
| glfw      |    ✔️    |                                                   |
| qhull     |    ✖️    | Can be disabled with `RAWRBOX_BUILD_UTIL_QHULL 0` |
| fmt       |    ✔️    | Used for formatting                               |
| utfcpp    |    ✔️    | Used for text rendering                           |
| stb/image |    ✔️    | Used for loading images                           |
| freetype2 |    ✔️    | Used for loading fonts                            |

# Building

## WINDOWS

### Required software

-   [GIT](https://git-scm.com/) or something similar to GIT
-   Download and install C++ Build Tools
-   Download and install a IDE, [Visual Code](https://code.visualstudio.com/) is recommended.
-   Download and install CONAN (> 2.0)
-   Download and setup [ninja](https://github.com/ninja-build/ninja/releases) (make sure it's in the enviroment path)

### IDE SETUP (Visual Code)

-   Open the workspace file and install the recommended extensions (at least the C++, CMAKE)
-   Run cmake configure
-   Select `[Visual Studio Build Tools <version> Release - amd64]` on the kit popup (for all of the projects)
-   Run the compiler by hitting F5
-   Hope it builds fine 🙏

## LINUX

### Required software

-   [PODMAN](https://podman.io/)

### IDE SETUP

-   TODO
