FROM ubuntu

ARG semgrep_version=0.113.0

WORKDIR /app

RUN apt update
RUN apt install -y curl sqlite3 libsqlite3-dev build-essential
RUN curl -L "https://github.com/returntocorp/semgrep/releases/download/v${semgrep_version}/semgrep-v${semgrep_version}-ubuntu-16.04.tgz" | tar xvz
RUN curl -L "https://github.com/returntocorp/semgrep/archive/refs/tags/v${semgrep_version}.tar.gz" | tar xvz
RUN cp /app/semgrep-files/semgrep_bridge_core.so /usr/lib/libsemgrep_bridge_core.so
