# Use with gcc
* When using the `g++-mapper-server` binary:
    * You can't use absolute pathing. Hence it is highly encouraged to keep a local
symlink to wherever your global gcm cache lives.
    * If the directory doesn't exist, it won't be made. Odd since it _does_ make
the directory locally at pwd when invoked without location.
    * Always use the `--noisy` flag. Else, it can fail silently.
