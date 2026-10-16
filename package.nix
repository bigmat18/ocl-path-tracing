{
  lib,
  stdenv,
  cmake,
  ninja,
  pkg-config,
  cpp-core-utils,
  etu-opengl,
  opencl-intercept-layer,
  ocl-icd,
  opencl-headers,
  opencl-clhpp,
  glfw,
  libGL,
  glm,
  tinyobjloader,
  stb,
  nlohmann_json,
}:

stdenv.mkDerivation {
  pname = "ocl-path-tracing";
  version = "0.1.0";

  nativeBuildInputs = [
    cmake
    ninja
    pkg-config
  ];

  buildInputs = [
    cpp-core-utils
    etu-opengl
    opencl-intercept-layer
    ocl-icd
    opencl-headers
    opencl-clhpp
    glfw
    libGL
    glm
    tinyobjloader
    stb
    nlohmann_json
  ];

  src = lib.cleanSourceWith {
    src = ./.;
    filter = path: type:
      let base = baseNameOf (toString path);
      in !(base == "build" || base == ".cache" || base == "test_build" || base == "external" || base == "result");
  };

  meta = with lib; {
    description = "OpenCL Path Tracing";
    license = licenses.mit;
    maintainers = [ "bigmat18" ];
    platforms = platforms.unix;
  };
}
