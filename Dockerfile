FROM alpine:3.16 as build

ARG semgrep_version=0.113.0

WORKDIR /app

RUN apk add curl gcc sqlite-dev libc-dev
RUN curl -L "https://github.com/returntocorp/semgrep/releases/download/v${semgrep_version}/semgrep-v${semgrep_version}-ubuntu-16.04.tgz" | tar xvz
RUN curl -L "https://github.com/returntocorp/semgrep/archive/refs/tags/v${semgrep_version}.tar.gz" | tar xvz
RUN cp /app/semgrep-files/semgrep_bridge_core.so /usr/lib/libsemgrep_bridge_core.so

COPY semgrep.c /app/semgrep.c

RUN gcc -g -fPIC -I /app/semgrep-${semgrep_version}/semgrep-core/src/cli-bridge -shared semgrep.c -o semgrep.so -l semgrep_bridge_core

FROM alpine:3.16

RUN apk add sqlite tree-sitter pcre libstdc++

COPY --from=build /app/semgrep.so /usr/lib/semgrep.so
COPY --from=build /app/semgrep-files/semgrep_bridge_core.so /usr/lib/libsemgrep_bridge_core.so

CMD ["/usr/bin/sqlite3"]