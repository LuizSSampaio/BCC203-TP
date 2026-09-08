{ pkgs, lib, config, inputs, ... }:

{
  # https://devenv.sh/packages/
  packages = [
    pkgs.git
    pkgs.gnumake
    pkgs.clang
  ];

  # https://devenv.sh/languages/
  languages.cplusplus = {
    enable = true;
    lsp.package = pkgs.clang;
  };

  # https://devenv.sh/git-hooks/
  git-hooks.hooks = {
    clang-format.enable = true;
    clang-tidy.enable = true;
  };

  # See full reference at https://devenv.sh/reference/options/
}
