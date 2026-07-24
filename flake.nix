{
  description = "ProtNES dev env";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
          ];
          buildInputs = [
            pkgs.gtest
          ];

          shellHook = ''
            if [[ $- == *i* ]] && [ -z "$IN_NIX_SHELL_ZSH" ]; then
              export IN_NIX_SHELL_ZSH=1
              exec ${pkgs.zsh}/bin/zsh
            fi
          '';
        };
      });
}
