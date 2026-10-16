{
  description = "OpenCL Path Tracing";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    cpp-core-utils = {
      url = "github:bigmat18/cpp-core-utils";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
    };
    etu-opengl = {
      url = "github:bigmat18/etu-opengl";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
      inputs.cpp-core-utils.follows = "cpp-core-utils";
    };
    opencl-intercept-layer = {
      url = "github:intel/opencl-intercept-layer";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      cpp-core-utils,
      etu-opengl,
      opencl-intercept-layer,
    }:
    flake-utils.lib.eachSystem
      [
        "x86_64-linux"
        "i686-linux"
        "aarch64-linux"
        "x86_64-darwin"
      ]
      (
        system:
        let
          pkgs = import nixpkgs {
            inherit system;
            overlays = [ ];
          };
          coreUtils = cpp-core-utils.packages.${system}.default;
          etuOpengl = etu-opengl.packages.${system}.default;
          openclInterceptLayer = pkgs.stdenv.mkDerivation {
            pname = "opencl-intercept-layer";
            version = "3.0.6";
            src = opencl-intercept-layer;
            nativeBuildInputs = with pkgs; [
              cmake
              ninja
            ];
          };
          ocl-path-tracing = pkgs.callPackage ./package.nix {
            cpp-core-utils = coreUtils;
            etu-opengl = etuOpengl;
            opencl-intercept-layer = openclInterceptLayer;
          };
        in
        {
          packages = {
            default = ocl-path-tracing;
            ocl-path-tracing = ocl-path-tracing;
            opencl-intercept-layer = openclInterceptLayer;
          };

          devShells.default = pkgs.mkShell rec {
            name = "ocl-path-tracing";

            packages = with pkgs; [
              gcc
              cmake
              ninja
              clang-tools
              pkg-config
              clinfo
              openclInterceptLayer
            ];

            buildInputs = with pkgs; [
              coreUtils
              etuOpengl
              openclInterceptLayer
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

            shellHook = ''
              mkdir -p external
              ln -sfn "${coreUtils}" external/cpp-core-utils
              ln -sfn "${etuOpengl}" external/etu-opengl
              ln -sfn "${openclInterceptLayer}" external/opencl-intercept-layer
              ln -sfn "${pkgs.ocl-icd}" external/opencl
              ln -sfn "${pkgs.opencl-headers}" external/opencl-headers
              ln -sfn "${pkgs.opencl-clhpp}" external/opencl-clhpp
              export CLI_OpenCLFileName="${pkgs.ocl-icd}/lib/libOpenCL.so.1"
              echo "ocl-path-tracing env ready (external/ symlinks populated)"
              if [[ $- == *i* ]]; then
                exec zsh
              fi
            '';
          };
        }
      );
}
