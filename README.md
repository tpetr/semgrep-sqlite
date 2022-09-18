# sqlite-semgrep

A sqlite extension for evaluating semgrep patterns.

```
sqlite-semgrep % docker build --platform amd64 -t sqlite-semgrep .
[+] Building 2.2s (16/16) FINISHED
 => [internal] load build definition from Dockerfile                                                                 0.0s
 => => transferring dockerfile: 900B                                                                                 0.0s
 => [internal] load .dockerignore                                                                                    0.0s
 => => transferring context: 2B                                                                                      0.0s
 => [internal] load metadata for docker.io/library/alpine:3.16                                                       0.3s
 => [build 1/8] FROM docker.io/library/alpine:3.16@sha256:bc41182d7ef5ffc53a40b044e725193bc10142a1243f395ee852a8d97  0.0s
 => [internal] load build context                                                                                    0.0s
 => => transferring context: 2.94kB                                                                                  0.0s
 => CACHED [build 2/8] WORKDIR /app                                                                                  0.0s
 => CACHED [build 3/8] RUN apk add curl gcc sqlite-dev libc-dev                                                      0.0s
 => CACHED [build 4/8] RUN curl -L "https://github.com/returntocorp/semgrep/releases/download/v0.113.0/semgrep-v0.1  0.0s
 => CACHED [build 5/8] RUN curl -L "https://github.com/returntocorp/semgrep/archive/refs/tags/v0.113.0.tar.gz" | ta  0.0s
 => CACHED [build 6/8] RUN cp /app/semgrep-files/semgrep_bridge_core.so /usr/lib/libsemgrep_bridge_core.so           0.0s
 => [build 7/8] COPY semgrep.c /app/semgrep.c                                                                        0.0s
 => [build 8/8] RUN gcc -g -fPIC -I /app/semgrep-0.113.0/semgrep-core/src/cli-bridge -shared semgrep.c -o semgrep.s  1.2s
 => CACHED [stage-1 2/4] RUN apk add sqlite tree-sitter pcre libstdc++                                               0.0s
 => [stage-1 3/4] COPY --from=build /app/semgrep.so /usr/lib/semgrep.so                                              0.0s
 => [stage-1 4/4] COPY --from=build /app/semgrep-files/semgrep_bridge_core.so /usr/lib/libsemgrep_bridge_core.so     0.2s
 => exporting to image                                                                                               0.2s
 => => exporting layers                                                                                              0.2s
 => => writing image sha256:841c4de2ec8fbe3fdd0dede4fe6be9cefa4cbaa93f6f5c5fbc763bddf23ee7c9                         0.0s
 => => naming to docker.io/library/sqlite-semgrep                                                                    0.0s

Use 'docker scan' to run Snyk tests against images to find vulnerabilities and learn how to fix them

sqlite-semgrep % docker run -it --rm sqlite-semgrep
WARNING: The requested image's platform (linux/amd64) does not match the detected host platform (linux/arm64/v8) and no specific platform was requested
SQLite version 3.38.5 2022-05-06 15:25:27
Enter ".help" for usage hints.
Connected to a transient in-memory database.
Use ".open FILENAME" to reopen on a persistent database.
sqlite> .load semgrep
sqlite> select semgrep_version();
semgrep-core version: 0.113.0, pfff: 0.42

sqlite> select semgrep('python', '$X == $X', 'if "bla" == "bla":');
{"matches":[{"rule_id":"-e/-f","location":{"path":"/tmp/sqlite-semgrep.XXPjFCgl","start":{"line":1,"col":4,"offset":3},"end":{"line":1,"col":18,"offset":17}},"extra":{"message":"","metavars":{"$X":{"start":{"line":1,"col":4,"offset":3},"end":{"line":1,"col":9,"offset":8},"abstract_content":"\"bla\""}}}}],"errors":[],"stats":{"okfiles":1,"errorfiles":0}}

sqlite> select semgrep('python', '$X == $X', 'if "bla" == "bla":') -> '$.matches[0].extra.metavars.$X.abstract_content';
"\"bla\""
```
