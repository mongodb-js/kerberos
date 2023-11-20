FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -y krb5-user

COPY krb5.conf /etc/krb5.conf

CMD [ "/app/.evergreen/mongodb-tests/run-tests.sh" ]
